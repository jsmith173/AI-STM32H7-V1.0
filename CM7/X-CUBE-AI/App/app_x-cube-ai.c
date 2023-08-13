
/**
  ******************************************************************************
  * @file    app_x-cube-ai.c
  * @author  X-CUBE-AI C code generator
  * @brief   AI program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

 /*
  * Description
  *   v1.0 - Minimum template to show how to use the Embedded Client API
  *          model. Only one input and one output is supported. All
  *          memory resources are allocated statically (AI_NETWORK_XX, defines
  *          are used).
  *          Re-target of the printf function is out-of-scope.
  *   v2.0 - add multiple IO and/or multiple heap support
  *
  *   For more information, see the embeded documentation:
  *
  *       [1] %X_CUBE_AI_DIR%/Documentation/index.html
  *
  *   X_CUBE_AI_DIR indicates the location where the X-CUBE-AI pack is installed
  *   typical : C:\Users\<user_name>\STM32Cube\Repository\STMicroelectronics\X-CUBE-AI\7.1.0
  */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#if defined ( __ICCARM__ )
#elif defined ( __CC_ARM ) || ( __GNUC__ )
#endif

/* System headers */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "app_x-cube-ai.h"
#include "main.h"
#include "ai_datatypes_defines.h"
#include "network.h"
#include "network_data.h"
#include "img_array.h"


/* Global handle to reference the instantiated C-model */
static ai_handle network = AI_HANDLE_NULL;

/* Global c-array to handle the activations buffer */
AI_ALIGNED(32)
static ai_u8 activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];

/* Array to store the data of the input tensor */
AI_ALIGNED(32)
static ai_float* in_data;
//static ai_float in_data[AI_NETWORK_IN_1_SIZE];
//static ai_u8 in_data[AI_NETWORK_IN_1_SIZE_BYTES]; 

/* c-array to store the data of the output tensor */
AI_ALIGNED(32)
static ai_float out_data[AI_NETWORK_OUT_1_SIZE];
/* static ai_u8 out_data[AI_NETWORK_OUT_1_SIZE_BYTES]; */

/* Array of pointer to manage the model's input/output tensors */
static ai_buffer *ai_input;
static ai_buffer *ai_output;
static float g_ai_output[AI_NETWORK_OUT_1_SIZE];

/* 
 * Bootstrap
 */
void MX_X_CUBE_AI_Init(void) 
{
  ai_error err;
  
  /* Create and initialize the c-model */
  const ai_handle acts[] = { activations };
  err = ai_network_create_and_init(&network, acts, NULL);
  if (err.type != AI_ERROR_NONE) {  };

  /* Reteive pointers to the model's input/output tensors */
  ai_input = ai_network_inputs_get(network, NULL);
  ai_output = ai_network_outputs_get(network, NULL);
}

/* 
 * Run inference
 */
int aiRun(const void *in_data, void *out_data) {
  ai_i32 n_batch;
  ai_error err;
  
  /* 1 - Update IO handlers with the data payload */
  ai_input[0].data = AI_HANDLE_PTR(in_data);
  ai_output[0].data = AI_HANDLE_PTR(out_data);

  /* 2 - Perform the inference */
  n_batch = ai_network_run(network, &ai_input[0], &ai_output[0]);
  if (n_batch != 1) {
      err = ai_network_get_error(network);
      //...
  };
  
  return 0;
}

uint8_t post_process(float *out_data)
{
    uint8_t prediction, i = 0;
    float max_val = -1;

    /* find max */
    for(i = 0; i < AI_NETWORK_OUT_1_SIZE; i++) 
    {
        if (max_val < out_data[i]) 
        {
            max_val = out_data[i];
            prediction = i;
        }
    }

    return prediction;
}

/* 
 * Example of main loop function
 */
int a, c=0, passed=1; 
float* img_array;

void acquire_and_process_data(ai_float* a)
{
	in_data = a;
}

void MX_X_CUBE_AI_Process() 
{
  int prediction;
  
  /* 1 - Acquire, pre-process and fill the input buffers */
  if (c == 0)
	  img_array = img_array0;
  else if (c == 1)
	  img_array = img_array1;
  else if (c == 2)
	  img_array = img_array2;
  
  acquire_and_process_data(img_array);

  /* 2 - Call inference engine */
  aiRun(in_data, out_data);

  /* 3 - Post-process the predictions */
  prediction = post_process(out_data);
  
  if (c != prediction && c < 3) 
	passed = 0;
  
  if (c == 2)
	a=1;
  
  c++;
  
}


#ifdef __cplusplus
}
#endif
