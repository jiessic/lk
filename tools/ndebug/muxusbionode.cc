/*
 * Copyright 2016 Google Inc. All Rights Reserved.
 * Author: gkalsi@google.com (Gurjant Kalsi)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "muxusbionode.h"
#include "usbionode.h"

namespace NDebug {

MuxUSBIONode::MuxUSBIONode(const sys_channel_t ch, USBIONode *usb) 
    : ch_(ch),
      usb_(usb),
      sem_(0) {}

MuxUSBIONode::~MuxUSBIONode() {}

IONodeResult MuxUSBIONode::readBuf(std::vector<uint8_t> *buf) {
    *buf = queue_.pop();
    return IONodeResult::Success;
}

IONodeResult MuxUSBIONode::writeBuf(const std::vector<uint8_t> &buf) {
    sem_.wait();
    // Write the channel as the first byte of the result.
    uint32_t channel = ch_;

    std::vector<uint8_t> outBuf(
        reinterpret_cast<uint8_t *>(&channel),
        reinterpret_cast<uint8_t *>(&channel) + sizeof(channel)
    );

    std::copy(buf.begin(), buf.end(), std::back_inserter(outBuf));

    return usb_->writeBuf(outBuf);
}

void MuxUSBIONode::queueBuf(const std::vector<uint8_t> &buf) {
    queue_.push(buf);
}

void MuxUSBIONode::signalBufAvail() {
    sem_.signal();
}

}  // namespace