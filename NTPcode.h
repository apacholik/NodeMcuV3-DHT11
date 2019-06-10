#ifndef Arduino_h
#include <Arduino.h>
#endif
#ifndef WiFi_h
#include <ESP8266WiFi.h>
#endif
#ifndef WIFIUDP_H
#include <WiFiUdp.h>
#endif

unsigned long getTimeFromNTP() {
  unsigned long startTime;
  int stopWhen = 10000;
  unsigned int localPort = 2390;      // local port to listen for UDP packets

  /* Don't hardwire the IP address or we won't get the benefits of the pool.
   *  Lookup the IP address for the host name instead */
  //IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
  IPAddress timeServerIP; // time.nist.gov NTP server address
  const char* ntpServerName = "europe.pool.ntp.org";
  
  const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
  
  byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
  
  // A UDP instance to let us send and receive packets over UDP
  WiFiUDP udp;
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 

  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(timeServerIP, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
  // wait to see if a reply is available
  
  Serial.print("Waiting for packet");
  int cb = udp.parsePacket();
  startTime = millis();
  while (!cb) {
    Serial.print(".");
    delay(100);
    cb = udp.parsePacket();
    if ((millis() - startTime) > stopWhen) {
      Serial.println("");
      Serial.println(String("Can't get time from NTP (waiting ") + String(stopWhen / 1000) + "s).");
      return 0;
    }
  }
  Serial.println("");
  Serial.println("We got packet");
  // We've received a packet, read the data from it
  udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

  //the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, esxtract the two words:

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  // Serial.print("Seconds since Jan 1 1900 = " );
  // Serial.println(secsSince1900);

  // now convert NTP time into everyday time:
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  unsigned long unixTime = secsSince1900 - seventyYears;
  return unixTime;
}
