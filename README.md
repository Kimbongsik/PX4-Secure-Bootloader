# PX4 Secure Boot & Firmware Update

CoT(Chaining of Trust) 형태로 구현한 Secure Bootloader

![1](https://github.com/user-attachments/assets/cac9cec8-20d6-4e63-a05d-5a14df945bbe)

[]()

# Introduction

- **지원 보드**: Pixhawk 6C mini
- **지원 프로세서**: STM32H743
- **개발 환경**: 리눅스(Linux)

# Getting Started

## 1. Replace PX4 Bootloader File

**1) 기존 PX4 프로젝트의 bootloader 링커 스크립트 파일을 교체**

- 위치
    
    ```bash
    boards/px4/fmu-v6c/scripts/bootloader_scrypt.ld
    ```
    

**2) 기존 PX4 프로젝트의 bootloader 소스코드 파일을 교체**

- 위치
    
    ```bash
    platforms/nuttx/src/bootloader/common/bl.c
    ```
    

## 2. Build PX4 Project

- **1.의 파일 교체 후, PX4 프로젝트 빌드**
- PX4 부트로더 빌드

```bash
make px4_fmu-v6c_bootloader.bin
```

- PX4 Firmware 빌드

```bash
make px4_fmu-v6c_default.bin
```

## 3. ImageBuilder

![2](https://github.com/user-attachments/assets/ac012a94-fa3b-47f1-8c31-c72e1b57e313)

- **ImgBuilder 폴더 하위에 PX4 Bootloader & PX4 Firmware 이미지 저장**
- **ImgBuilder의 main.cpp 파일에서 경로 수정**

## 4. Execution

- **ImgBuilder 실행**
    - fin_img.bin : 최종 바이너리 파일 (2048KB)
    

## 5. Fusing

- Debugger를 사용하여 플래시 메모리에 최종 바이너리 파일을 fusing
- FMU Debug Port 사용

![3](https://github.com/user-attachments/assets/76c9fba0-7828-402c-ae22-e7a3ec7ceffb)