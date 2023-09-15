//// LoRaWan devices automatically base64 encode data sent
// Sometimes nice just to view what original payload was sent
//
// This can help when writing your own decoder
//
//

 function Decoder(bytes, port, uplink_info) {
  var decoded = {};

 decoded.PM2_5 = bytes[0]*100 +  bytes[1];
 decoded.T = bytes[2] +  bytes[3]/100;
 decoded.TH = bytes[4] +  bytes[5]/100;
            
  return decoded;

 }

// And in the integration section of the Helium console use this JSON
// For an Adafruit.io integration which is expecting JSON like { "value" : "56" }
//
// { "value": {{decoded.payload.value}} }