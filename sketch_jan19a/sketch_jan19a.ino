#include <WiFi.h>
#include <time.h> // Time library

// WiFi settings
const char* ssid = "Daniel_iPhone"; // WiFi network name
const char* password = "12345"; // WiFi password

WiFiServer server(80); // Create an HTTP server on port 80

// Message storage
String chatMessages[10];  // Store up to 10 recent messages
String timestamps[10];    // Store timestamps for each message
int messageIndex = 0;     // Index for adding new messages

// LED pin
const int ledPin = 22;

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Set up the LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Turn off the LED by default

  // Set up NTP (Network Time Protocol)
  configTime(0, 0, "pool.ntp.org");
  Serial.println("Syncing time...");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nTime synchronized!");

  server.begin(); // Start the server
}

void loop() {
  WiFiClient client = server.available(); // Check for client connection
  if (!client) {
    return;
  }

  // Read the client's request
  String request = "";
  while (client.available()) {
    char c = client.read();
    request += c;
  }
  Serial.println("Request:");
  Serial.println(request);

  // Ignore favicon requests
  if (request.indexOf("GET /favicon.ico") != -1) {
    return;
  }

  // Check if there is a new message from the user
  if (request.indexOf("POST /chat") != -1) {
    String newMessage = extractMessage(request); // Extract the message from the request
    if (newMessage.length() > 0) {
      addMessage(newMessage); // Add the message to the storage
      flashLed(); // Notify with LED
    }
  }

  // Send the HTML page
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<title>ESP32 Chat</title>");
  client.println("<style>");
  client.println("body { font-family: Arial; text-align: center; }");
  client.println("textarea { width: 80%; height: 100px; }");
  client.println(".button { padding: 10px 20px; font-size: 16px; }");
  client.println("</style>");
  client.println("</head>");
  client.println("<body>");
  client.println("<h1>ESP32 Chat</h1>");

  // Display the messages
  client.println("<h2>Messages:</h2>");
  client.println("<div style='text-align: left; margin: 0 auto; width: 80%; border: 1px solid #ccc; padding: 10px;'>");
  for (int i = 0; i < 10; i++) {
    if (chatMessages[i].length() > 0) {
      client.println("<b>[" + timestamps[i] + "]</b> " + chatMessages[i] + "<br>");
    }
  }
  client.println("</div>");

  // Form to send a message
  client.println("<h2>Send a Message:</h2>");
  client.println("<form action='/chat' method='POST'>");
  client.println("<textarea name='message' placeholder='Write your message here...'></textarea><br>");
  client.println("<input type='submit' value='Send' class='button'>");
  client.println("</form>");

  client.println("</body>");
  client.println("</html>");
}

// Function to extract a message from an HTTP request
String extractMessage(String request) {
  int bodyStart = request.indexOf("\r\n\r\n"); // Find the start of the request body
  if (bodyStart == -1) {
    return "";
  }
  String body = request.substring(bodyStart + 4);
  int messageStart = body.indexOf("message="); // Find the "message=" parameter
  if (messageStart == -1) {
    return "";
  }
  String message = body.substring(messageStart + 8);
  message.replace("+", " "); // Replace '+' with spaces
  return urlDecode(message); // Decode URL-encoded characters
}

// Function to add a message to the storage
void addMessage(String newMessage) {
  chatMessages[messageIndex] = newMessage;

  // Get the current time
  time_t now = time(nullptr);
  struct tm* timeInfo = localtime(&now);
  char timeString[20];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeInfo);

  timestamps[messageIndex] = String(timeString);
  Serial.println("Added message: " + newMessage + " at " + timestamps[messageIndex]);

  messageIndex = (messageIndex + 1) % 10; // Move to the next index (circular buffer)
}

// Function to flash the LED
void flashLed() {
  for (int i = 0; i < 3; i++) { // Flash 3 times
    digitalWrite(ledPin, HIGH);
    delay(300);
    digitalWrite(ledPin, LOW);
    delay(300);
  }
}

// Function to decode URL-encoded characters
String urlDecode(String input) {
  String decoded = "";
  for (int i = 0; i < input.length(); i++) {
    if (input[i] == '%') {
      int code = strtol(input.substring(i + 1, i + 3).c_str(), NULL, 16);
      decoded += char(code);
      i += 2;
    } else if (input[i] == '+') {
      decoded += ' '; // Replace '+' with space
    } else {
      decoded += input[i];
    }
  }
  return decoded;
}
