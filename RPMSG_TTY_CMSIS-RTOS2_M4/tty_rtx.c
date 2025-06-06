/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Modifications Copyright (c) 2016 ARM Germany GmbH. All rights reserved.
 */

#include <stdlib.h>
#include <string.h>
#include "rpmsg.h"
#include "rpmsg_ext.h"
#include "rpmsg_rtos.h"
#include "board.h"
#include "mu_imx.h"
#include "cmsis_os2.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "retarget_io_user.h"

#define PRINTF printf

/* Globals */
static char app_buf[512]; /* Each RPMSG buffer can carry less than 512 payload */

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////
void TTYThread (void *arg);

typedef struct the_message
{
  uint32_t  DATA;
} THE_MESSAGE, * THE_MESSAGE_PTR;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

void TTYThread (void *arg)
{
  int result;
  struct remote_device *rdev = NULL;
  struct rpmsg_channel *app_chnl = NULL;
  void *rx_buf;
  int len;
  unsigned long src;
  void *tx_buf;
  unsigned long size;
  unsigned int count = 0;

  /* Print the initial banner */
  PRINTF("\r\nRPMSG TTY RTX Demo...\r\n");

  /* RPMSG Init as REMOTE */
  PRINTF("RPMSG Init as Remote\r\n");
  result = rpmsg_rtos_init(0 /*REMOTE_CPU_ID*/, &rdev, RPMSG_MASTER, &app_chnl);
  assert(result == 0);

  /* reinitialize debug uart */
  InitRetargetIOUSART();

  PRINTF("Name service handshake is done, M4 has setup a rpmsg channel [%ld ---> %ld]\r\n", app_chnl->src, app_chnl->dst);

  while (true)
  {
    /* Get RPMsg rx buffer with message */
    result = rpmsg_rtos_recv_nocopy(app_chnl->rp_ept, &rx_buf, &len, &src, 0xFFFFFFFF);
    assert(result == 0);

    /* Copy string from RPMsg rx buffer */
    assert(len < sizeof(app_buf));
    memcpy(app_buf, rx_buf, len);
    app_buf[len] = 0; /* End string by '\0' */

    if ((len == 2) && (app_buf[0] == 0xd) && (app_buf[1] == 0xa))
      PRINTF("Get New Line From Master Side\r\n");
    else
      PRINTF("Get Message From Master Side: \"%s\" [len : %d] (%d)\r\n", app_buf, len, count);

    tx_buf = rpmsg_rtos_alloc_tx_buffer(app_chnl->rp_ept, &size);
    assert(tx_buf);
    snprintf(tx_buf,22, "Hello from M4! (%04d)", count);

    /* Send message with nocopy */
    result = rpmsg_rtos_send_nocopy(app_chnl->rp_ept, tx_buf, 22, src);
    assert(result == 0);

    /* Release held RPMsg rx buffer */
    result = rpmsg_rtos_recv_nocopy_free(app_chnl->rp_ept, rx_buf);
    assert(result == 0);

    count++;
  }
}

int main(void)
{
  hardware_init();

  SystemCoreClockUpdate();
  
  InitRetargetIOUSART();

  /* Initialize CMSIS-RTOS */
  osKernelInitialize(); 
  
  /* Create new thread */
  osThreadNew(TTYThread, NULL, NULL);              

  /* Start thread execution */ 
  osKernelStart();               
  
  /* Infinite loop */
  while(true);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
