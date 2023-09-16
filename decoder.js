 function Decoder(bytes, port, uplink_info) {
  var decoded = {};
 decoded.PM1 = (bytes[0]*256 +  bytes[1])/10.0;
 decoded.PM2_5 = (bytes[2]*256 +  bytes[3])/10.0;
 decoded.PM10 = (bytes[4]*256 +  bytes[5])/10.0;
 decoded.T = bytes[6] +  bytes[7]/100;
 decoded.TH = bytes[8] +  bytes[9]/100;
 decoded.RSSI = normalizedPayload.gateways[0].rssi.toFixed(0);
 decoded.SNR = normalizedPayload.gateways[0].snr.toFixed(0);
 decoded.FREQUENCY = normalizedPayload.frequency;
  return decoded;

 }
