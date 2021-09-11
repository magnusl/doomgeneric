//doomgeneric for soso os

#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <stdio.h>
#include <stdbool.h>

#include <os/log_api.h>

#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

static unsigned char convertToDoomKey(unsigned int key){
  return 0;
}

static void handleKeyInput(){

}


void DG_Init(){

}

void DG_DrawFrame() {
  log_notice("Draw frame!");
}

void DG_SleepMs(uint32_t ms) {

}

uint32_t DG_GetTicksMs() {
  return 0;
}

int DG_GetKey(int* pressed, unsigned char* doomKey) {
  return 0;
}

void DG_SetWindowTitle(const char * title) {

}
