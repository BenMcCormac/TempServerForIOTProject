//temp function to simulate temp sensor
String getTemp() 
{
  DHT.read(DHT11_PIN);
  String temp = String(DHT.temperature);
  return temp;
}

String getHumid()
{
  DHT.read(DHT11_PIN);
  String humid = String(DHT.humidity);
  return humid;
}