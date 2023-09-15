 function Decoder(bytes, port, uplink_info) {
  var decoded = {};
 decoded.PM1 = bytes[0]*100 +  bytes[1];
 decoded.PM2_5 = bytes[2]*100 +  bytes[3];
 decoded.PM10 = bytes[4]*100 +  bytes[5];
 decoded.T = bytes[6] +  bytes[7]/100;
 decoded.TH = bytes[8] +  bytes[9]/100;
 decoded.RSSI = normalizedPayload.gateways[0].rssi.toFixed(0);
 decoded.SNR = normalizedPayload.gateways[0].snr.toFixed(0);
 decoded.FREQUENCY = normalizedPayload.frequency;
  return decoded;
 }