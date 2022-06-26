// LoRaWan devices automatically base64 encode data sent
// Sometimes nice just to view what original payload was sent
//
// This can help when writing your own decoder
//
//

 function Decoder(bytes, port, uplink_info) {
  var decoded = [];
  try{
 decoded.push({
                "field": "PM2_5",
                "value": bytes[0]*100 +  bytes[1]
            });
            
  return decoded;
  } catch (err) {
  return 'Decoder: ' + err.name + " : " + err.message;;
  }
 }

// And in the integration section of the Helium console use this JSON
// For an Adafruit.io integration which is expecting JSON like { "value" : "56" }
//
// { "value": {{decoded.payload.value}} }