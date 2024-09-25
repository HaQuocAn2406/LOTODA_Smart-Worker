#include "_SPIFFS.h"
// Data mydata;
void writeFile(const char *path, const char *message)
{
  File file = SPIFFS.open(path, FILE_WRITE);

  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }

  if (file.print(message))
  {
    Serial.println("File written successfully");
  }
  else
  {
    Serial.println("Failed to write file");
  }

  file.close(); // Don't forget to close the file
}

String readFile(fs::SPIFFSFS &fs, const char *path)
{
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory())
  {
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while (file.available())
  {
    fileContent = file.readStringUntil('\n');
    break;
  }
  return fileContent;
}

void writeStruct(const char * path, const Data &data) {
  File file = SPIFFS.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  file.write((const uint8_t*)&data, sizeof(data));
  file.close();
}

void readStruct(const char * path, Data &data) {
  File file = SPIFFS.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  file.read((uint8_t*)&data, sizeof(data));
  file.close();
}