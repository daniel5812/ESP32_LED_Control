#include <WiFi.h>
#include <time.h> // ספריית זמן

// הגדרות WiFi
const char* ssid = "Daniel_iPhone"; // שם רשת ה-WiFi
const char* password = "86998699"; // סיסמת ה-WiFi

WiFiServer server(80); // יצירת שרת HTTP על פורט 80

// מאגר ההודעות
String chatMessages[10];  // אחסון עד 10 הודעות אחרונות
String timestamps[10];    // שמירת השעות של כל הודעה
int messageIndex = 0;     // אינדקס להוספת הודעות חדשות

// פין הנורה
const int ledPin = 22;

void setup() {
  Serial.begin(115200);

  // חיבור ל-WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // הגדרת פין הנורה
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // כיבוי הנורה כברירת מחדל

  // הגדרת זמן NTP
  configTime(0, 0, "pool.ntp.org");
  Serial.println("Syncing time...");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nTime synchronized!");

  server.begin(); // הפעלת השרת
}

void loop() {
  WiFiClient client = server.available(); // בדיקת חיבור לקוח
  if (!client) {
    return;
  }

  // קריאת כל הבקשה לדיבוג
  String request = "";
  while (client.available()) {
    char c = client.read();
    request += c;
  }
  Serial.println("Request:");
  Serial.println(request);

  // טיפול בבקשות favicon
  if (request.indexOf("GET /favicon.ico") != -1) {
    return; // התעלמות מבקשת favicon
  }

  // בדיקת אם יש הודעה חדשה מהמשתמש
  if (request.indexOf("POST /chat") != -1) {
    String newMessage = extractMessage(request); // חילוץ ההודעה מהבקשה
    if (newMessage.length() > 0) {
      addMessage(newMessage); // הוספת ההודעה למאגר
      flashLed(); // התראה על הודעה חדשה
    }
  }

  // שליחת עמוד HTML
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

  // הצגת ההודעות
  client.println("<h2>Messages:</h2>");
  client.println("<div style='text-align: left; margin: 0 auto; width: 80%; border: 1px solid #ccc; padding: 10px;'>");
  for (int i = 0; i < 10; i++) {
    if (chatMessages[i].length() > 0) {
      client.println("<b>[" + timestamps[i] + "]</b> " + chatMessages[i] + "<br>");
    }
  }
  client.println("</div>");

  // טופס לשליחת הודעה
  client.println("<h2>Send a Message:</h2>");
  client.println("<form action='/chat' method='POST'>");
  client.println("<textarea name='message' placeholder='Write your message here...'></textarea><br>");
  client.println("<input type='submit' value='Send' class='button'>");
  client.println("</form>");

  client.println("</body>");
  client.println("</html>");
}

// פונקציה לחילוץ הודעה מבקשת HTTP
String extractMessage(String request) {
  int bodyStart = request.indexOf("\r\n\r\n");
  if (bodyStart == -1) {
    return "";
  }
  String body = request.substring(bodyStart + 4);
  int messageStart = body.indexOf("message=");
  if (messageStart == -1) {
    return "";
  }
  String message = body.substring(messageStart + 8);
  message.replace("+", " ");
  return urlDecode(message);
}

// פונקציה להוספת הודעה למאגר
void addMessage(String newMessage) {
  chatMessages[messageIndex] = newMessage;

  // קבלת השעה הנוכחית
  time_t now = time(nullptr);
  struct tm* timeInfo = localtime(&now);
  char timeString[20];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeInfo);

  timestamps[messageIndex] = String(timeString);
  Serial.println("Added message: " + newMessage + " at " + timestamps[messageIndex]);

  messageIndex = (messageIndex + 1) % 10;
}

// פונקציה להבהוב הנורה
void flashLed() {
  for (int i = 0; i < 3; i++) { // הבהוב 3 פעמים
    digitalWrite(ledPin, HIGH);
    delay(300);
    digitalWrite(ledPin, LOW);
    delay(300);
  }
}

// פונקציה לפענוח URL
String urlDecode(String input) {
  String decoded = "";
  for (int i = 0; i < input.length(); i++) {
    if (input[i] == '%') {
      int code = strtol(input.substring(i + 1, i + 3).c_str(), NULL, 16);
      decoded += char(code);
      i += 2;
    } else if (input[i] == '+') {
      decoded += ' ';
    } else {
      decoded += input[i];
    }
  }
  return decoded;
}
