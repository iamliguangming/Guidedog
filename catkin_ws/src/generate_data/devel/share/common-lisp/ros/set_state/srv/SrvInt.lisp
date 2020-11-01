; Auto-generated. Do not edit!


(cl:in-package set_state-srv)


;//! \htmlinclude SrvInt-request.msg.html

(cl:defclass <SrvInt-request> (roslisp-msg-protocol:ros-message)
  ((request_int
    :reader request_int
    :initarg :request_int
    :type cl:integer
    :initform 0))
)

(cl:defclass SrvInt-request (<SrvInt-request>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <SrvInt-request>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'SrvInt-request)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name set_state-srv:<SrvInt-request> is deprecated: use set_state-srv:SrvInt-request instead.")))

(cl:ensure-generic-function 'request_int-val :lambda-list '(m))
(cl:defmethod request_int-val ((m <SrvInt-request>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader set_state-srv:request_int-val is deprecated.  Use set_state-srv:request_int instead.")
  (request_int m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <SrvInt-request>) ostream)
  "Serializes a message object of type '<SrvInt-request>"
  (cl:let* ((signed (cl:slot-value msg 'request_int)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <SrvInt-request>) istream)
  "Deserializes a message object of type '<SrvInt-request>"
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'request_int) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<SrvInt-request>)))
  "Returns string type for a service object of type '<SrvInt-request>"
  "set_state/SrvIntRequest")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'SrvInt-request)))
  "Returns string type for a service object of type 'SrvInt-request"
  "set_state/SrvIntRequest")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<SrvInt-request>)))
  "Returns md5sum for a message object of type '<SrvInt-request>"
  "b0ea3c0a4031127490817459e301f614")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'SrvInt-request)))
  "Returns md5sum for a message object of type 'SrvInt-request"
  "b0ea3c0a4031127490817459e301f614")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<SrvInt-request>)))
  "Returns full string definition for message of type '<SrvInt-request>"
  (cl:format cl:nil "int32 request_int~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'SrvInt-request)))
  "Returns full string definition for message of type 'SrvInt-request"
  (cl:format cl:nil "int32 request_int~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <SrvInt-request>))
  (cl:+ 0
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <SrvInt-request>))
  "Converts a ROS message object to a list"
  (cl:list 'SrvInt-request
    (cl:cons ':request_int (request_int msg))
))
;//! \htmlinclude SrvInt-response.msg.html

(cl:defclass <SrvInt-response> (roslisp-msg-protocol:ros-message)
  ((response_int
    :reader response_int
    :initarg :response_int
    :type cl:integer
    :initform 0))
)

(cl:defclass SrvInt-response (<SrvInt-response>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <SrvInt-response>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'SrvInt-response)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name set_state-srv:<SrvInt-response> is deprecated: use set_state-srv:SrvInt-response instead.")))

(cl:ensure-generic-function 'response_int-val :lambda-list '(m))
(cl:defmethod response_int-val ((m <SrvInt-response>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader set_state-srv:response_int-val is deprecated.  Use set_state-srv:response_int instead.")
  (response_int m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <SrvInt-response>) ostream)
  "Serializes a message object of type '<SrvInt-response>"
  (cl:let* ((signed (cl:slot-value msg 'response_int)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <SrvInt-response>) istream)
  "Deserializes a message object of type '<SrvInt-response>"
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'response_int) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<SrvInt-response>)))
  "Returns string type for a service object of type '<SrvInt-response>"
  "set_state/SrvIntResponse")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'SrvInt-response)))
  "Returns string type for a service object of type 'SrvInt-response"
  "set_state/SrvIntResponse")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<SrvInt-response>)))
  "Returns md5sum for a message object of type '<SrvInt-response>"
  "b0ea3c0a4031127490817459e301f614")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'SrvInt-response)))
  "Returns md5sum for a message object of type 'SrvInt-response"
  "b0ea3c0a4031127490817459e301f614")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<SrvInt-response>)))
  "Returns full string definition for message of type '<SrvInt-response>"
  (cl:format cl:nil "int32 response_int~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'SrvInt-response)))
  "Returns full string definition for message of type 'SrvInt-response"
  (cl:format cl:nil "int32 response_int~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <SrvInt-response>))
  (cl:+ 0
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <SrvInt-response>))
  "Converts a ROS message object to a list"
  (cl:list 'SrvInt-response
    (cl:cons ':response_int (response_int msg))
))
(cl:defmethod roslisp-msg-protocol:service-request-type ((msg (cl:eql 'SrvInt)))
  'SrvInt-request)
(cl:defmethod roslisp-msg-protocol:service-response-type ((msg (cl:eql 'SrvInt)))
  'SrvInt-response)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'SrvInt)))
  "Returns string type for a service object of type '<SrvInt>"
  "set_state/SrvInt")