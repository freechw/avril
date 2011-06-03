// Copyright 2011 Olivier Gillet.
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------
//
// Event queue.

#ifndef AVRLIB_UI_EVENT_QUEUE_
#define AVRLIB_UI_EVENT_QUEUE_

#include "avrlib/base.h"
#include "avrlib/op.h"
#include "avrlib/ring_buffer.h"

namespace avrlib {

enum ControlType {
  POT = 0,
  ENCODER = 1,
  SWITCH = 2
};

struct Event {
  uint8_t control_type;
  uint8_t control_id;
  uint8_t value;
};

template<uint8_t size = 32>
class EventQueue {
 public:
  enum {
    buffer_size = size,
    data_size = 16,
  };
  typedef uint16_t Value;
  typedef EventQueue<size> Me;
   
  EventQueue() { }
  
  static void Flush() {
    events_.Flush();
  };
  
  static void AddEvent(uint8_t control_type, uint8_t id, uint8_t data) {
    last_event_time_ = milliseconds();
    Word v;
    v.bytes[0] = U8ShiftLeft4(control_type) | id;
    v.bytes[1] = data;
    events_.Overwrite(v);
  }
  
  static uint8_t available() {
    return events_.available();
  }
  
  static uint32_t idle_time() {
    uint32_t now = milliseconds();
    return static_cast<uint16_t>(now - last_event_time_) >> 8;
  }
  
  static Event PullEvent() {
    Event e;
    Word v = events_.ImmediateRead();
    e.control_type = U8ShiftRight4(v.bytes[0]);
    e.control_id = v.bytes[0] & 0x0f;
    e.value = v.bytes[1];
    return e;
  }
  
 private:
  static uint32_t last_event_time_;
  static RingBuffer<Me> events_;
};

/* static */
template<uint8_t size>
RingBuffer<EventQueue<size> > EventQueue<size>::events_;

/* static */
template<uint8_t size>
uint32_t EventQueue<size>::last_event_time_;

}  // namespace avrlib

#endif AVRLIB_UI_EVENT_QUEUE_