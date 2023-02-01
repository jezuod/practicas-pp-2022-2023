# OS Name (Linux or Darwin)
OSUPPER = $(shell uname -s 2>/dev/null | tr [:lower:] [:upper:])
OSLOWER = $(shell uname -s 2>/dev/null | tr [:upper:] [:lower:])

# Flags to detect 32-bit or 64-bit OS platform
OS_SIZE = $(shell uname -m | sed -e "s/i.86/32/" -e "s/x86_64/64/")
OS_ARCH = $(shell uname -m | sed -e "s/i386/i686/")


# These flags will override any settings
ifeq ($(i386),1)
	OS_SIZE = 32
	OS_ARCH = i686
endif

ifeq ($(x86_64),1)
	OS_SIZE = 64
	OS_ARCH = x86_64
endif

# Flags to detect either a Linux system (linux) or Mac OSX (darwin)
DARWIN = $(strip $(findstring DARWIN, $(OSUPPER)))

# Location of the CUDA Toolkit binaries and libraries
CUDA_PATH       ?= /usr/local/cuda
CUDA_INC_PATH   ?= $(CUDA_PATH)/include
CUDA_BIN_PATH   ?= $(CUDA_PATH)/bin
ifneq ($(DARWIN),)
  CUDA_LIB_PATH  ?= $(CUDA_PATH)/lib
else
  ifeq ($(OS_SIZE),32)
    CUDA_LIB_PATH  ?= $(CUDA_PATH)/lib
  else
    CUDA_LIB_PATH  ?= $(CUDA_PATH)/lib64
  endif
endif

# Common binaries
NVCC            ?= $(CUDA_BIN_PATH)/nvcc
GCC             ?= g++
#
EXTRA_NVCCFLAGS ?= -w -use_fast_math -Wno-deprecated-gpu-targets
EXTRA_LDFLAGS   ?= -w -use_fast_math -Wno-deprecated-gpu-targets

GENCODE_SM35    := -gencode arch=compute_35,code=sm_35
GENCODE_SM37    := -gencode arch=compute_37,code=sm_37
GENCODE_SM50    := -gencode arch=compute_50,code=sm_50
GENCODE_SM52    := -gencode arch=compute_52,code=sm_52
GENCODE_SM60    := -gencode arch=compute_60,code=sm_60
GENCODE_SM61    := -gencode arch=compute_61,code=sm_61
GENCODE_SM70    := -gencode arch=compute_70,code=sm_70
GENCODE_SM75    := -gencode arch=compute_75,code=sm_75
GENCODE_FLAGS   := $(GENCODE_SM35) $(GENCODE_SM37) 

# OS-specific build flags
ifneq ($(DARWIN),) 
      LDFLAGS   := -Xlinker $(CUDA_LIB_PATH) -L$(CUDA_LIB_PATH) -fopenmp -w
      CCFLAGS   := -arch $(OS_ARCH) 
else
  ifeq ($(OS_SIZE),32)
      LDFLAGS   := -L$(CUDA_LIB_PATH) -L. 
      CCFLAGS   := -m32 -w
  else
      LDFLAGS   := -L. -L$(CUDA_LIB_PATH) -L. -lcudart -lcudadevrt -lcusolver 
      CCFLAGS   := -m64 -w
  endif
endif

# OS-architecture specific flags
ifeq ($(OS_SIZE),32)
      NVCCFLAGS := -m32
else
      NVCCFLAGS := -m64
endif

# Debug build flags
ifeq ($(dbg),1)
      CCFLAGS   += -g 
      NVCCFLAGS += -g -G
      TARGET    := debug
else
      CCFLAGS	+= -O3 -fopenmp 
      ICCFLAGS  += -O3 -DPLOTITERH -fopenmp 
      NVCCFLAGS += -O3 -Wno-deprecated-gpu-targets
      TARGET    := release
endif

EXTRA_CCFLAGS := -I./Header
# Target rules
all: build

build: fm

file.o: ./Src/file.cpp
	$(GCC) $(CCFLAGS) $(EXTRA_CCFLAGS) -o $@ -c -w -fopenmp $<

matrix.o: ./Src/matrix.cpp
	$(GCC) $(CCFLAGS) $(EXTRA_CCFLAGS) -o $@ -c -w -fopenmp $<

wtime.o: ./Src/wtime.cpp
	$(GCC) $(CCFLAGS) $(EXTRA_CCFLAGS) -o $@ -c -w -fopenmp $<

main.o: ./Src/main.cpp
	$(GCC) $(CCFLAGS) -I$(CUDA_INC_PATH) $(EXTRA_CCFLAGS) -o $@ -c -w -fopenmp $<

factorR.o: ./Src/factorR.cpp
	$(GCC) $(CCFLAGS) -I$(CUDA_INC_PATH) $(EXTRA_CCFLAGS) -o $@ -c -w -fopenmp $<

det.o: ./Src/determinante_cuda.cu
	$(NVCC) $(NVCCFLAGS) $(GENCODE_FLAGS) $(EXTRA_CCFLAGS) $(INCLUDES) -o $@ -c -w -Xptxas -dlcm=ca -use_fast_math $<
	
fm: det.o wtime.o factorR.o file.o matrix.o main.o
	$(GCC) $(INCLUDES) $(CCFLAGS) -o $@ $+ $(LDFLAGS) $(EXTRA_LDFLAGS)  

run: build
	./fm

clean:
	rm -f fm *.o 

data:
	rm -rf log log_* 
