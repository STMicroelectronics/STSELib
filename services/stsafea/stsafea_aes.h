/**
  ******************************************************************************
  * @file    stsafea_aes.h
  * @author  CS Application Team
  * @brief   STSAFE-A services for symmetric key cryptography (header)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef STSAFEA_AES_H
#define STSAFEA_AES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define STSAFEA_MAC_SIZE 4U

#include "services/stsafea/stsafea_aes_ecb.h"
#include "services/stsafea/stsafea_aes_ccm.h"
#include "services/stsafea/stsafea_aes_gcm.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STSAFEA_AES_H */
