// Auto-generated. Do not edit!

// (in-package set_state.srv)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;

//-----------------------------------------------------------


//-----------------------------------------------------------

class SrvIntRequest {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.request_int = null;
    }
    else {
      if (initObj.hasOwnProperty('request_int')) {
        this.request_int = initObj.request_int
      }
      else {
        this.request_int = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type SrvIntRequest
    // Serialize message field [request_int]
    bufferOffset = _serializer.int32(obj.request_int, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type SrvIntRequest
    let len;
    let data = new SrvIntRequest(null);
    // Deserialize message field [request_int]
    data.request_int = _deserializer.int32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 4;
  }

  static datatype() {
    // Returns string type for a service object
    return 'set_state/SrvIntRequest';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '3178d1d7ec6cecacf3be2661f2752839';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    int32 request_int
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new SrvIntRequest(null);
    if (msg.request_int !== undefined) {
      resolved.request_int = msg.request_int;
    }
    else {
      resolved.request_int = 0
    }

    return resolved;
    }
};

class SrvIntResponse {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.response_int = null;
    }
    else {
      if (initObj.hasOwnProperty('response_int')) {
        this.response_int = initObj.response_int
      }
      else {
        this.response_int = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type SrvIntResponse
    // Serialize message field [response_int]
    bufferOffset = _serializer.int32(obj.response_int, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type SrvIntResponse
    let len;
    let data = new SrvIntResponse(null);
    // Deserialize message field [response_int]
    data.response_int = _deserializer.int32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 4;
  }

  static datatype() {
    // Returns string type for a service object
    return 'set_state/SrvIntResponse';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'eaa684da2e3648981ee22ce4b464424e';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    int32 response_int
    
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new SrvIntResponse(null);
    if (msg.response_int !== undefined) {
      resolved.response_int = msg.response_int;
    }
    else {
      resolved.response_int = 0
    }

    return resolved;
    }
};

module.exports = {
  Request: SrvIntRequest,
  Response: SrvIntResponse,
  md5sum() { return 'b0ea3c0a4031127490817459e301f614'; },
  datatype() { return 'set_state/SrvInt'; }
};
