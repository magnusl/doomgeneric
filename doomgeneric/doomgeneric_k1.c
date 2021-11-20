//doomgeneric for soso os

#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <ddk/devices/bga.h>
#include <ddk/devices/pci.h>
#include <ddk/devices/dev.h>
#include <sys/mailbox.h>
#include <kernel_headers/keyboard.h>

#define KEYQUEUE_SIZE 16

static void* s_Framebuffer;
static size_t s_FramebufferSize;
static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;
static int s_KeyboardHandle;

static unsigned char convertToDoomKey(unsigned int key){
  return 0;
}

static inline int Map_Byte_E0(uint8_t Byte_Value, int* pressed, int* key) {
  switch(Byte_Value & 0x7F) {
  case 0x48: *key = KEY_UPARROW; break;
  case 0x4B: *key = KEY_LEFTARROW; break;
  case 0x50: *key = KEY_DOWNARROW; break;
  case 0x4D: *key = KEY_RIGHTARROW; break;
  default:
    return 0;
  } // if
  *pressed = (Byte_Value & 0x80) != 0x80;
  return 1;
}

static inline int Map_Byte(uint8_t Byte_Value, int* pressed, int* key) {
  switch(Byte_Value & 0x7F) {
  case 0x01: *key = KEY_ESCAPE; break;
  case 0x1c: *key = KEY_ENTER; break;
  case 0x36: *key = KEY_RSHIFT; break;
  case 0x39: *key = KEY_USE; break;
  case 0x1D: *key = KEY_FIRE; break;
  default:
    return 0;
  } // switch

  *pressed = (Byte_Value & 0x80) != 0x80;
  return 1;
} // Map_Byte

static void handleKeyInput() {
 
  while(1) {
    keyboard_event ev;
    int mail_size = mailbox_recv(s_KeyboardHandle, &ev, sizeof(ev), MSG_NOBLOCK, NULL);
    if (mail_size <= 0) {
      return;
    }

    if (mail_size != sizeof(keyboard_event)) {
      return;
    }

    int key, pressed, ok;
    if (ev.scancode.data[0] == 0xe0) {
        ok = Map_Byte_E0(ev.scancode.data[1], &pressed, &key);
    }
    else {
        ok = Map_Byte(ev.scancode.data[0], &pressed, &key);
    } // else

    if (ok) {
      unsigned short keyData = (pressed << 8) | key;
      s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
      s_KeyQueueWriteIndex++;
      s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
    }
  } // while
}

void DG_Init(){

  pci_init();
  s_Framebuffer = bga_init(DOOMGENERIC_RESX, DOOMGENERIC_RESY, &s_FramebufferSize);

  ace_t ace;
    ace.trustee.trustee_type = TRUSTEE_TYPE_ALL;
    ace.access_mask = PROT_WRITE;

    dacl_t acl = dacl_create(&ace, 1);
    s_KeyboardHandle = mailbox_create(acl, "System\\TTY_Keyboard", PROT_READ);
    dev_attach(DEV_PS2_KEYBOARD, s_KeyboardHandle);
}

void DG_DrawFrame() {
  memcpy(s_Framebuffer, DG_ScreenBuffer, DOOMGENERIC_RESX * DOOMGENERIC_RESY * sizeof(uint32_t));
  handleKeyInput();
}

void DG_SleepMs(uint32_t ms) {
  usleep(ms * 1000);
}

uint32_t DG_GetTicksMs() {
  return get_tick_count();
}

int DG_GetKey(int* pressed, unsigned char* doomKey) {
  if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex){
    //key queue is empty
    return 0;
  } else{
    unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
    s_KeyQueueReadIndex++;
    s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

    *pressed = keyData >> 8;
    *doomKey = keyData & 0xFF;

    return 1;
  }

  return 0;
}

void DG_SetWindowTitle(const char * title) {

}
