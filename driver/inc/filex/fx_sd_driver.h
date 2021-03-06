// /**************************************************************************/
// /*                                                                        */
// /*       Copyright (c) Microsoft Corporation. All rights reserved.        */
// /*                                                                        */
// /*       This software is licensed under the Microsoft Software License   */
// /*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
// /*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
// /*       and in the root directory of this software.                      */
// /*                                                                        */
// /**************************************************************************/

// /* Define to prevent recursive inclusion -------------------------------------*/
// #ifndef FX_STM32_SD_DRIVER_H
// #define FX_STM32_SD_DRIVER_H

// #ifdef __cplusplus
// extern "C"
// {
// #endif

// /* Includes ------------------------------------------------------------------*/
// #include "fx_api.h"

// /* please include the correct BSP/SD driver header file */
// #include "../../../common/inc/common/device.h"

// /* Exported types ------------------------------------------------------------*/
// /* USER CODE BEGIN ET */

// /* USER CODE END ET */

// /* Exported constants --------------------------------------------------------*/
// #define DEFAULT_TIMEOUT (10 * TX_TIMER_TICKS_PER_SECOND)

// #define DEFAULT_SECTOR_SIZE 512

// #define ENABLE_CACHE_MAINTENANCE 1

// #define FX_DRIVER_CALLS_SD_INIT 0

//     /* USER CODE BEGIN EC */

//     /* USER CODE END EC */

//     /* Exported macro ------------------------------------------------------------*/
//     /* USER CODE BEGIN EM */

//     /* USER CODE END EM */

//     /* Exported functions prototypes ---------------------------------------------*/
//     VOID fx_sd_driver(FX_MEDIA *media_ptr);

//     /* USER CODE BEGIN EFP */
//     void fx_sd_driver_device_set(SdDevice *fx_device);
//     /* USER CODE END EFP */

//     /* Private defines -----------------------------------------------------------*/
//     /* USER CODE BEGIN PD */

//     /* USER CODE END PD */

//     /* USER CODE BEGIN 1 */

//     /* USER CODE END 1 */

// #ifdef __cplusplus
// }
// #endif

// #endif /* FX_STM32_SD_DRIVER_H */
