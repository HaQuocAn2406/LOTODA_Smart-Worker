#include "sever.h"
void LOTODA_Server::writeFile(const char *path, const char *message)
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

String LOTODA_Server::readFile(fs::SPIFFSFS &fs, const char *path)
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

void LOTODA_Server::Handle_Network()
{
    on("/networkinfo", HTTP_GET, [this](AsyncWebServerRequest *request)
       {
        String ssid = readFile(SPIFFS,"/ssid.txt");
        String pass = readFile(SPIFFS,"/pass.txt");
        String jsonResponse = "{";
        jsonResponse += "\"ssid\":\"" + ssid + "\",";
        jsonResponse += "\"password\":\"" + pass + "\",";
        jsonResponse += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
        jsonResponse += "\"mac\":\"" + WiFi.macAddress() + "\"";
        jsonResponse += "}";
        request->send(200, "application/json", jsonResponse); });
}

void LOTODA_Server::Handle_NetworkAP()
{
    on("/networkinfo", HTTP_GET, [this](AsyncWebServerRequest *request)
       {
        String jsonResponse = "{";
        jsonResponse += "\"ssid\":\"" + WiFi.softAPSSID() + "\",";
        jsonResponse += "\"ip\":\"" + WiFi.softAPIP().toString() + "\",";
        jsonResponse += "\"mac\":\"" + WiFi.macAddress() + "\"";
        jsonResponse += "}";
        request->send(200, "application/json", jsonResponse); });
}

void LOTODA_Server::Handle_Reboot()
{
    on("/reboot", HTTP_GET, [this](AsyncWebServerRequest *request)
       {
        Serial.println("REBOOT.....");
        request->send(200, "text/plain","ESP REBOOTING.....");
        ESP.restart(); });
}

void LOTODA_Server::Handle_SubmitForm()
{
    on("/submit", HTTP_POST, [this](AsyncWebServerRequest *request)
       {
        Serial.println("Get Done");
        int params = request->params();
        for (int i = 0; i < params; i++)
        {
            AsyncWebParameter *p = request->getParam(i);
            if (p->isPost())
            {
                // HTTP POST ssid value
                if (p->name() == "ssidInput")
                {
                    String ssid = p->value().c_str();
                    Serial.print("SSID set to: ");
                    Serial.println(ssid);
                    writeFile("/ssid.txt",ssid.c_str());
                }
                // HTTP POST pass value
                if (p->name() == "password")
                {
                    String pass = p->value().c_str();
                    Serial.print("Password set to: ");
                    Serial.println(pass);
                    writeFile("/pass.txt",pass.c_str()); 
                }
            }
        } 
        ESP.restart(); });
}

void LOTODA_Server::WiFi_Config()
{
    on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
       { request->send(SPIFFS, "/wificonfig.html"); });
    on("/style.css", HTTP_GET, [this](AsyncWebServerRequest *request)
       { request->send(SPIFFS, "/style.css"); });
    on("/script.js", HTTP_GET, [this](AsyncWebServerRequest *request)
       { request->send(SPIFFS, "/wifi_script.js"); });
    Handle_NetworkAP();
    Handle_SubmitForm();
    begin();
}
