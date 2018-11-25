#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include "KeyValueFlash.h"

WiFiManager wifiManager;
KeyValueFlash config;

// ------------------------------------------------------------------------------------------- //
// WebServer related globals
// ------------------------------------------------------------------------------------------- //
// Served pages
#define CONFIG_EDIT_URL "/config/edit"
#define CONFIG_VIEW_URL "/config/view"

const char   *serverName      = "something"; // WebServer hostname, MUST be all lowercase
const short  serverPort      = 80;           // WebServer port

const String serverPageTitle = "Welcome to " + String(serverName) + "!";
const String tdStyle         = "style=\"border: 1px solid black;padding-right: 10px;padding-left: 10px\"";

ESP8266WebServer server;

// ------------------------------------------------------------------------------------------- //
// WebServer related functions prototypes
// ------------------------------------------------------------------------------------------- //
void   configureWebServerHandlersAndBegin();

void   handleRoot();
void   handleNotFound();
void   handleConfigViewPage();
void   handleConfigEditForm();
void   handleConfigEditFormPOST();

String openHeadAndBody();
String closeBodyAndHtml();
String makeRedirectButton(String text, String url);

String getWiFiInfos();

// ------------------------------------------------------------------------------------------- //
// Misc prototypes
// ------------------------------------------------------------------------------------------- //
bool isButtonPressed();

// ------------------------------------------------------------------------------------------- //
// Global Config Variables
// ------------------------------------------------------------------------------------------- //
#define EXAMPLE_CONFIG_VARIABLE "exampleVariable" // Example variable
String  exampleConfigVariable;

// ------------------------------------------------------------------------------------------- //
// setup
// ------------------------------------------------------------------------------------------- //
void setup()
{
	Serial.begin(115200);
	while (!Serial) {
		yield();
	}
	Serial.println("Hello!");

	// If the button is pressed when booting, start in AP mode to give the last IP
    if (isButtonPressed()) {
    	// start in AP mode with web server and tralala
    } else {
    	wifiManager.autoConnect(serverName);
    }

	// Print local IP address
	Serial.println("WiFi connected.");
	Serial.print("IP address:\t"); Serial.println(WiFi.localIP());

	// Save IP address to flash memory
	if (!config.get("lastIP").equals(WiFi.localIP().toString())){
		config.set("lastIP", WiFi.localIP().toString());
	}

	// Start mDNS (to be reachable via "http://THING_NAME.local"
	if (!MDNS.begin(serverName)) {
		Serial.println("Error setting up MDNS responder!");
	}
	Serial.printf("mDNS responder started [%s.local]\n", serverName);

	// Initialize the web server and its handlers
	configureWebServerHandlersAndBegin();
	Serial.printf("HTTP server started [http://%s.local]\n", serverName);
}

void loop() {
	server.handleClient();
}

void loadConfig() {
	Serial.println("Loading configuration variables from flash memory");

	exampleConfigVariable = config.get(EXAMPLE_CONFIG_VARIABLE);
}

bool isButtonPressed() {
	return false;
}

// ------------------------------------------------------------------------------------------- //
// WebServer functions
// ------------------------------------------------------------------------------------------- //

void configureWebServerHandlersAndBegin() {
	server.on("/", HTTP_GET, handleRoot);

	server.on(CONFIG_VIEW_URL, HTTP_GET, handleConfigViewPage);
	server.on(CONFIG_EDIT_URL, HTTP_GET, handleConfigEditForm);
	server.on(CONFIG_EDIT_URL, HTTP_POST, handleConfigEditFormPOST);

	server.onNotFound(handleNotFound);

	server.begin(serverPort);
}

void handleRoot() {
	String content = openHeadAndBody();

    content += getWiFiInfos();

	// View config button
    content += "<BR><a href=\"";
    content += CONFIG_VIEW_URL;
	content += "\"><button type=\"button\">View configuration</button></a>";

    content += closeBodyAndHtml();

	server.send(200, "text/html", content);
}

void handleNotFound() {
	server.send(404, "text/plain", "404: Not found");
}

void handleConfigViewPage() {
	// Send the head and open the body
	String content = openHeadAndBody();

	content += "<H2>Configuration Variables</H2>";

	// Open table and display header
	content += "<table><tr>";
	content += "<th " + tdStyle + ">Key</th>";
	content += "<th " + tdStyle + ">Value</th>";
	content += "</tr>";

	// ----- Config Variables -----

	// ExampleConfigVariable
	content += "<tr>";
	content += "<td " + tdStyle + ">" + EXAMPLE_CONFIG_VARIABLE + "</td>";
	content += "<td " + tdStyle + ">" + exampleConfigVariable + "</td>";
	content += "</tr>";

	// ----------------------------

	// Close table and display buttons
	content += "</table><BR>";
	content += "<div display=\"inline-block\">";
	content += makeRedirectButton("Back to main page", "/");
	content += makeRedirectButton("Edit configuration", CONFIG_EDIT_URL);
	content += "</div>";

	// Close the body and html
	content += closeBodyAndHtml();

	server.send(200, "text/html", content);
}

void handleConfigEditForm() {
	// Send the head and open the body
	String content = openHeadAndBody();

	// Start the form
	content += "<form action=\"";
	content += CONFIG_EDIT_URL;
	content += "\" method=\"POST\">";
	content += "<H2>Configuration Parameters</H2>";

	// Open table and display header
	content += "<table><tr>";
	content += "<th " + tdStyle + ">Key</th>";
	content += "<th " + tdStyle + ">Value</th>";
	content += "</tr>";

	// ----- Config Variables -----

	// ExampleConfigVariable
	content += "<tr>";
	content += "<td " + tdStyle + ">" + EXAMPLE_CONFIG_VARIABLE + "</td>";
	content += "<td " + tdStyle + ">"
			+  "<input type=\"text\" name=\"exampleConfigVariable\" placeholder=\"Example Config Variable Hint\" "
			+ "size=\"60\" value=\"" + exampleConfigVariable  + "\" ></td>";
	content += "</tr>";

	// ----------------------------

	// Close table
	content += "</table>";

	// Navigation buttons
	content += "<BR>";
	content += "<div display=\"inline-block\">";
	content += makeRedirectButton("Cancel", CONFIG_VIEW_URL);

	// Close the form, body and html
	content += "<input type=\"submit\" value=\"Save configuration\"></form>";
	content += "</div>";
	content += closeBodyAndHtml();

	server.send(200, "text/html", content);
}

void handleConfigEditFormPOST() {

	// Save EXAMPLE_CONFIG_VARIABLE
	if (server.hasArg(EXAMPLE_CONFIG_VARIABLE)) {
		config.set(EXAMPLE_CONFIG_VARIABLE, server.arg(EXAMPLE_CONFIG_VARIABLE));
	}

	// Redirect to the configuration page
	server.sendHeader("Location", CONFIG_VIEW_URL);
	server.send(303);
}

// ------------------------------------------------------------------------------------------- //

String openHeadAndBody() {
    String content = "";
	content += "<!DOCTYPE html><html>";
    content += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
    content += "<link rel=\"icon\" href=\"data:,\">";
    content += "<body>";
    content += "<h1>" + serverPageTitle + "</h1>";

    return content;
}

String closeBodyAndHtml() {
	return String("</body></html>");
}

String makeRedirectButton(String text, String url) {
	return String("<a href=\"" + url + "\"><button type=\"button\">" + text + "</button></a>");
}

String getWiFiInfos() {
	String content = "";

	content += "<H2>WiFi Connection</H2>";
	content +="<table>";
	content +="<tr>";
	content +="<td " + tdStyle + ">WiFi SSID</td>";
	content +="<td " + tdStyle + ">" + WiFi.SSID() + "</td>";
	content +="</tr>";
	content += "<td " + tdStyle + ">IP Address</td>";
	content +="<td "  + tdStyle + ">" + WiFi.localIP().toString() + "</td>";
	content +="</tr>";
	content +="</table>";

	return content;
}
