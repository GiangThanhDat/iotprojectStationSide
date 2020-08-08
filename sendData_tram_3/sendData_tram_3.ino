#include<DHT.h>            
const int DHTPIN = 2;      
const int DHTTYPE = DHT11;  

DHT dht(DHTPIN, DHTTYPE);

/*========================== CODE ĐÃ CHẠY NGON ĐỀ NGHỊ KHÔNG SỮA HAY CAN THIỆP VÀO BẤT CỨ THỨ GÌ TRONG FILE NÀY ===============================
 * Tác Giả : Giang Thành Đạt
 * Chỉnh sữa cuối cùng:22:55 8/8/2020
*/

/**
 * Kết nối Arduino với ESP8266
 * Arduino =========== ESP8266-01s
 * P0(RX) -------------> TX
 * P1(TX) -------------> RX
 *                -----> VCC
 * 3.3V   -------|
 *                -----> EN
 * GND    -------------> GND
 * ===========================
*/

#define delaytime 1000  // Mỗi lệnh AT gửi cách nhau 1ms

String wifiName = "Thanh Dat";
String Password = "4849398892";

const int NUMBER_OF_measures = 2;

String station_id = "3"; // mả trạm
/**
 * -----------------------------------
* CAMBIEN - ma_cambien | ten_cambien
*           01         | DHT-11
* ------------------------------------
* ------------------------------------
* DAILUONG - ma_dailuong|ten_dailuong
*            01         |Nhiệt Độ
*            02         |Độ Ẩm
* 
*/
String measures[NUMBER_OF_measures] = {"1_1","1_2"}; 
String values[NUMBER_OF_measures];

const String HOST = "thandatiotproject.herokuapp.com";

// khởi tạo Esp8266-01s
void Esp01sInit(){  
  sendCmd("AT\r\n",delaytime);
  sendCmd("AT+RST\r\n",delaytime); // reset moduole
  sendCmd("AT+CWMODE=1\r\n",delaytime); // Access point Mode  
  const String JoinwifiName = String("AT+CWJAP=\"") + wifiName + "\",\"" +  Password + "\"\r\n";// https://stackoverflow.com/questions/23936246/error-invalid-operands-of-types-const-char-35-and-const-char-2-to-binar
  sendCmd(JoinwifiName,delaytime); // Đăng nhập vào mạng wi-fi  
  sendCmd("AT+CIPMUX=0\r\n",delaytime); // Chọn chế độ kênh đơn
}


// Hàm gửi lệnh lên ESP-01s
void sendCmd(String cmd, int delayTime){
  Serial.print(cmd);  
  delay(delayTime);  
}


String getRequestString(String url, String measures[], String values[]){
  String result = "GET /" + url;
  result += "&" + station_id + "_" + measures[0] + "=" + values[0];      
  for(int i = 1 ; i < NUMBER_OF_measures; i++){    
      result += "&" + station_id + "_" + measures[i] + "=" +values[i];      
  }
  return result + " HTTP/1.1\r\nHost: " + HOST + "\r\n\r\n";
}
/*
 * Test gửi dữ liệu lên kênh thingspeak
  AT+CIPSTART="TCP","184.106.153.149",80<CR><LF>
  AT+CIPSEND=48<CR><LF>
  GET /update?api_key=0ZSTF6WMADP4GB75&field1=30\r\nHOST:184.106.153.149<CR><LF>
*/

/*
 *  AT+CIPSTART="TCP","gthanhdatpro.000webhostapp.com",80<CR><LF>
 *  AT+CIPSEND=73
 *  GET /index.php?temp=90 HTTP/1.1<CR><LF>HOST: gthanhdatpro.000webhostapp.com<CR><LF>
 *  
  */

  
/*
 * 
 getRequest = GET /update?api_key=0ZSTF6WMADP4GB75&field1=30<CR><LF>
 HOST = 184.106.153.149
 getRequest = GET /index.php?temp=90 HTTP/1.1\r\nHOST: gthanhdatpro.000webhostapp.com\r\n
 GET /collect/store&6801_1_1=105&6801_1_2=45&6801_2_1=69 HTTP/1.1<CR><LF>HOST: gthanhdatpro.000webhostapp.com<CR><LF>
*/
void sendRequest(String getRequest){
 // chuẩn bị các tập lệnh
  String makeTCPConnect = String("AT+CIPSTART=\"TCP\",\"") + HOST + "\",80\r\n"; // Tạo kết nối TCP: AT+CIPSTART="TCP","184.106.153.149",80<CR><LF>
  String sendLength = String("AT+CIPSEND=") + getRequest.length() + "\r\n"; //Tính độ dài chuổi GET: AT-CIPSEND=48
 
  // gửi đi
  sendCmd(makeTCPConnect,delaytime); 
  sendCmd(sendLength,delaytime);
  sendCmd(getRequest,delaytime);

  //đóng kết nối
  sendCmd("AT+CIPCLOSE\r\n",0);
}

void setup(){
  delay(3000);
  Serial.begin(9600); // Baud rate of esp8266  
  Esp01sInit(); 
  dht.begin();
//  Serial.println("Ket noi thanh cong");
//  camBienMQ2.begin();
}  




void loop(){  
    float nhiet_do = dht.readTemperature();
    float do_am = dht.readHumidity();     
    values[0] = String(nhiet_do);
    values[1] = String(do_am);
    sendRequest(getRequestString("collect/store",measures,values));
//  sendRequest("GET /collect/store&6801_1_1="+A+"&6801_1_2="+B+"&6801_2_1="+C+" HTTP/1.1\r\nHost: " + HOST + "\r\n\r\n");
  //và tiếp tục gửi dử liệu lên sau 1 phút
  delay(800);
}

   
  
