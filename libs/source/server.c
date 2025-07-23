// server.c - VERSÃO CORRIGIDA E MELHORADA

#include "server.h"
#include "globals.h"
#include "pico/time.h" // Adicionado para a função de alerta no display
#include <string.h>
#include <stdio.h>

// O seu HTML_BODY está ótimo, permanece o mesmo.

static float lastMinAHT10 = 0, lastMaxAHT10 = 0;
static float lastMinBMP280 = 0, lastMaxBMP280 = 0;
static float lastMinUmid = 0, lastMaxUmid = 0;
static float lastMinPress = 0, lastMaxPress = 0;
static char lastMsg[256] = "";



const char HTML_BODY[] =  "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n  <meta charset=\"UTF-8\">\r\n  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n  <title>Estação Meteorológica</title>\r\n  <style>\r\n    body {\r\n      font-family: 'Segoe UI', sans-serif;\r\n      text-align: center;\r\n      background: linear-gradient(135deg, #fdfefe, #fefefc);\r\n      padding: 20px;\r\n      color: #2c3e50;\r\n    }\r\n    h1, h2, h3, label { color: #417775; }\r\n    .linha-divisoria {\r\n      border: none;\r\n      border-top: 3px solid #7abcb8;\r\n      margin: 10px auto 30px auto;\r\n      width: 80%;\r\n    }\r\n    .input-box {\r\n      padding: 5px;\r\n      margin: 2px;\r\n      width: 140px;\r\n      border: 2px solid #ccc;\r\n      border-radius: 10px;\r\n      font-size: 16px;\r\n      box-shadow: inset 1px 1px 3px rgba(0, 0, 0, 0.05);\r\n      transition: border-color 0.3s;\r\n    }\r\n    .input-box:focus {\r\n      outline: none;\r\n      border-color: #007BFF;\r\n    }\r\n    button {\r\n      padding: 10px 25px;\r\n      margin-top: 15px;\r\n      font-size: 16px;\r\n      border: none;\r\n      border-radius: 10px;\r\n      background-color: #417775;\r\n      color: #fff;\r\n      cursor: pointer;\r\n      transition: background-color 0.3s ease, transform 0.2s ease;\r\n    }\r\n    button:hover {\r\n      background-color: #417775;\r\n      transform: scale(1.03);\r\n    }\r\n    #graficos {\r\n      display: flex;\r\n      justify-content: center;\r\n      flex-wrap: wrap;\r\n      gap: 30px;\r\n      margin: 40px auto;\r\n    }\r\n    .grafico-container {\r\n      flex: 1 1 400px;\r\n      max-width: 500px;\r\n    }\r\n    .container-duplas {\r\n      display: flex;\r\n      justify-content: center;\r\n      gap: 10px;\r\n      margin-top: 10px;\r\n    }\r\n    .dupla {\r\n      display: flex;\r\n      flex-direction: column;\r\n      gap: 12px;\r\n      min-width: 50px;\r\n    }\r\n    .tabela-info {\r\n      margin: 20px auto;\r\n      border-collapse: collapse;\r\n      font-size: 16px;\r\n      color: #417775;\r\n    }\r\n    .cell {\r\n      padding: 10px 15px;\r\n      border: 1px solid #7abcb8;\r\n    }\r\n    .header-row {\r\n      background-color: #f1f8e9;\r\n    }\r\n    .painel-limites {\r\n      width: 680px;\r\n      margin: 10px auto;\r\n      padding: 10px;\r\n      background: linear-gradient(135deg, #e0f7fa, #f1f8e9);\r\n      border-radius: 25px;\r\n      border: 3px solid #7abcb8;\r\n      box-shadow: 0 4px 12px rgba(0,0,0,0.1);\r\n    }\r\n    .painel-geral {\r\n      display: flex;\r\n      justify-content: center;\r\n      align-items: flex-start;\r\n      gap: 30px;\r\n      margin: 40px auto;\r\n      max-width: 1200px;\r\n    }\r\n    .col-esq {\r\n      display: flex;\r\n      flex-direction: column;\r\n      align-items: center;\r\n      gap: 20px;\r\n    }\r\n    .log-box {\r\n      width: 300px;\r\n      height: 400px;\r\n      overflow-y: auto;\r\n      border: 2px solid #7abcb8;\r\n      background: #f9f9f9;\r\n      border-radius: 15px;\r\n      padding: 15px;\r\n      font-size: 14px;\r\n      text-align: left;\r\n      box-shadow: inset 0 0 6px rgba(0,0,0,0.1);\r\n      margin-top: 25px;\r\n    }\r\n    .log-msg {\r\n      margin-bottom: 8px;\r\n      border-bottom: 1px solid #ddd;\r\n      padding-bottom: 4px;\r\n    }\r\n    .col-dir {\r\n      display: flex;\r\n      flex-direction: column;\r\n      align-items: flex-start;\r\n      margin-left: 75px;\r\n    }\r\n@media (max-width: 768px) {\r\n  .painel-geral {\r\n    flex-direction: column;\r\n    align-items: stretch;\r\n    padding: 0 10px;\r\n  }\r\n\r\n  .col-esq, .col-dir {\r\n    width: 100%;\r\n    margin: 0;\r\n    align-items: center;\r\n  }\r\n\r\n  .col-dir {\r\n    display: flex;\r\n    justify-content: center;\r\n  }\r\n\r\n  .log-box {\r\n    max-width: 90%;\r\n  }\r\n\r\n  .container-duplas {\r\n    flex-direction: column;\r\n    align-items: center;\r\n  }\r\n\r\n  .grafico-container {\r\n    width: 100%;\r\n    max-width: 100%;\r\n  }\r\n\r\n  .tabela-info {\r\n    width: 100%;\r\n    font-size: 14px;\r\n  }\r\n\r\n  .painel-limites {\r\n    width: 100%;\r\n    box-sizing: border-box;\r\n  }\r\n}\r\n\r\n\r\n  </style>\r\n  <script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\r\n  <script>\r\n    let temp = [], humid = [], press = [], time = [], graficoA, graficoB, graficoC;\r\n    const maxLogMsgs = 20;\r\n\r\n    function addLog(text) {\r\n      const logBox = document.getElementById('logBox');\r\n      const div = document.createElement('div');\r\n      div.className = 'log-msg';\r\n      div.textContent = text;\r\n      logBox.appendChild(div);\r\n      while (logBox.children.length > maxLogMsgs) {\r\n        logBox.removeChild(logBox.firstChild);\r\n      }\r\n      const shouldScroll = logBox.scrollTop + logBox.clientHeight >= logBox.scrollHeight - 20;\r\n      if (shouldScroll) {\r\n        logBox.scrollTop = logBox.scrollHeight;\r\n      }\r\n    }\r\n\r\n    function atualizar() {\r\n      fetch('/dados')\r\n        .then(res => res.json())\r\n        .then(data => {\r\n          time.push(data.time);\r\n          temp.push(data.temp);\r\n          humid.push(data.humid);\r\n          press.push(data.press);\r\n\r\n          document.getElementById('valaht').innerText = data.aht.toFixed(2);\r\n          document.getElementById('valbmp').innerText = data.bmp.toFixed(2);\r\n          document.getElementById('valUmid').innerText = data.humid.toFixed(2);\r\n          document.getElementById('valPress').innerText = data.press.toFixed(2);\r\n          document.getElementById('valAlt').innerText = data.alt.toFixed(2);\r\n\r\n          if (time.length > 60) {\r\n            time.shift(); temp.shift(); humid.shift(); press.shift();\r\n          }\r\n\r\n          [graficoA, graficoB, graficoC].forEach((g, i) => {\r\n            if (!g) return;\r\n            g.data.labels = [...time];\r\n            g.data.datasets[0].data = i === 0 ? [...temp] : i === 1 ? [...humid] : [...press];\r\n            g.update();\r\n          });\r\n        });\r\n    }\r\n\r\n    function atualizarLog() {\r\n      fetch('/log')\r\n        .then(res => res.json())\r\n        .then(data => {\r\n          if (data.msg !== \"0\") {\r\n            const now = new Date();\r\n            const timestamp = now.toLocaleTimeString();\r\n            addLog(`${timestamp}: ${data.msg}`);\r\n          }\r\n        });\r\n    }\r\n\r\n    function lim() {\r\n      fetch('/limites')\r\n        .then(res => res.json())\r\n        .then(data => {\r\n          if (!data) return;\r\n          const map = {\r\n            'AHT10': 'aht',\r\n            'BMP280': 'bmp',\r\n            'Umid': 'Umid',\r\n            'Press': 'Press'\r\n          };\r\n          Object.entries(map).forEach(([jsonKey, idSuffix]) => {\r\n            document.getElementById(`min${idSuffix}`).value = data[`min${jsonKey}`] ?? '';\r\n            document.getElementById(`max${idSuffix}`).value = data[`max${jsonKey}`] ?? '';\r\n            document.getElementById(`offset${idSuffix}`).value = data[`offset${jsonKey}`] ?? '';\r\n          });\r\n        });\r\n    }\r\n\r\n    function set_lim() {\r\n      const limites = {};\r\n      const map = {\r\n        'AHT10': 'aht',\r\n        'BMP280': 'bmp',\r\n        'Umid': 'Umid',\r\n        'Press': 'Press'\r\n      };\r\n      Object.entries(map).forEach(([jsonKey, idSuffix]) => {\r\n        ['min', 'max', 'offset'].forEach(prefix => {\r\n          limites[`${prefix}${jsonKey}`] = document.getElementById(`${prefix}${idSuffix}`).value;\r\n        });\r\n      });\r\n      fetch(`/configurar-limites?` + new URLSearchParams(limites));\r\n    }\r\n\r\n    function zerar_offset() {\r\n      fetch('/zerar').then(() => lim());\r\n    }\r\n\r\n    window.onload = () => {\r\n      function criarGrafico(id, label, cor, bg, legY) {\r\n        return new Chart(document.getElementById(id).getContext('2d'), {\r\n          type: 'line',\r\n          data: {\r\n            labels: [],\r\n            datasets: [{\r\n              label,\r\n              data: [],\r\n              borderColor: cor,\r\n              backgroundColor: bg,\r\n              tension: 0.3,\r\n              fill: true\r\n            }]\r\n          },\r\n          options: {\r\n            animation: { duration: 0 },\r\n            scales: {\r\n              x: { title: { display: true, text: 'Tempo (s)' } },\r\n              y: { beginAtZero: true, title: { display: true, text: legY } }\r\n            }\r\n          }\r\n        });\r\n      }\r\n\r\n      graficoA = criarGrafico('graficoA', 'Temperatura Média (°C)', '#007BFF', 'rgba(0,123,255,0.1)', 'Temperatura Média (°C)');\r\n      graficoB = criarGrafico('graficoB', 'Umidade (%)', '#FF5722', 'rgba(255,87,34,0.1)', 'Umidade (%)');\r\n      graficoC = criarGrafico('graficoC', 'Pressão Atmosférica (Pa)', '#4CAF50', 'rgba(76,175,80,0.1)', 'Pressão Atmosférica (Pa)');\r\n\r\n      lim();\r\n      setInterval(atualizar, 1000);\r\n      setInterval(atualizarLog, 5000);\r\n    };\r\n  </script>\r\n</head>\r\n<body>\r\n  <h1>Estação Meteorológica</h1>\r\n  <hr class=\"linha-divisoria\">\r\n\r\n  <div id=\"graficos\">\r\n    <div class=\"grafico-container\"><h3>Temperatura</h3><canvas id=\"graficoA\"></canvas></div>\r\n    <div class=\"grafico-container\"><h3>Umidade</h3><canvas id=\"graficoB\"></canvas></div>\r\n    <div class=\"grafico-container\"><h3>Pressão Atmosférica</h3><canvas id=\"graficoC\"></canvas></div>\r\n  </div>\r\n\r\n  <div class=\"painel-geral\">\r\n    <div class=\"col-esq\">\r\n      <table class=\"tabela-info\">\r\n        <tr class=\"header-row\">\r\n          <th class=\"cell\">AHT10 (°C)</th>\r\n          <th class=\"cell\">BMP280 (°C)</th>\r\n          <th class=\"cell\">Umid (%)</th>\r\n          <th class=\"cell\">Press Atm (Pa)</th>\r\n          <th class=\"cell\">Alt (m)</th>\r\n        </tr>\r\n        <tr>\r\n          <td class=\"cell\" id=\"valaht\">--</td>\r\n          <td class=\"cell\" id=\"valbmp\">--</td>\r\n          <td class=\"cell\" id=\"valUmid\">--</td>\r\n          <td class=\"cell\" id=\"valPress\">--</td>\r\n          <td class=\"cell\" id=\"valAlt\">--</td>\r\n        </tr>\r\n      </table>\r\n\r\n      <div class=\"painel-limites\">\r\n        <div class=\"container-duplas\">\r\n          <div class=\"dupla\">\r\n            <label><b>MIN AHT10 (°C):</b><input class=\"input-box\" type=\"number\" id=\"minaht\"></label>\r\n            <label><b>MAX AHT10 (°C):</b><input class=\"input-box\" type=\"number\" id=\"maxaht\"></label>\r\n            <label><b>OFFSET AHT10 (°C):</b><input class=\"input-box\" type=\"number\" id=\"offsetaht\"></label>\r\n          </div>\r\n          <div class=\"dupla\">\r\n            <label><b>MIN BMP280 (°C):</b><input class=\"input-box\" type=\"number\" id=\"minbmp\"></label>\r\n            <label><b>MAX BMP280 (°C):</b><input class=\"input-box\" type=\"number\" id=\"maxbmp\"></label>\r\n            <label><b>OFFSET BMP280 (°C):</b><input class=\"input-box\" type=\"number\" id=\"offsetbmp\"></label>\r\n          </div>\r\n          <div class=\"dupla\">\r\n            <label><b>MIN Umidade:</b><input class=\"input-box\" type=\"number\" id=\"minUmid\"></label>\r\n            <label><b>MAX Umidade:</b><input class=\"input-box\" type=\"number\" id=\"maxUmid\"></label>\r\n            <label><b>OFFSET Umidade:</b><input class=\"input-box\" type=\"number\" id=\"offsetUmid\"></label>\r\n          </div>\r\n          <div class=\"dupla\">\r\n            <label><b>MIN Pressão:</b><input class=\"input-box\" type=\"number\" id=\"minPress\"></label>\r\n            <label><b>MAX Pressão:</b><input class=\"input-box\" type=\"number\" id=\"maxPress\"></label>\r\n            <label><b>OFFSET Pressão:</b><input class=\"input-box\" type=\"number\" id=\"offsetPress\"></label>\r\n          </div>\r\n        </div>\r\n        <button onclick=\"set_lim()\">Atualizar Limites</button>\r\n        <button style=\"background-color:#336B87\" onclick=\"zerar_offset()\">Zerar Offset</button>\r\n      </div>\r\n    </div>\r\n\r\n    <div class=\"col-dir\">\r\n      <div class=\"log-box\" id=\"logBox\"></div>\r\n    </div>\r\n  </div>\r\n</body>\r\n</html>\r\n";

struct http_state
{
    char response[11500];
    size_t len;
    size_t sent;
};

static err_t http_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct http_state *hs = (struct http_state *)arg;
    hs->sent += len;
    if (hs->sent >= hs->len)
    {
        tcp_close(tpcb);
        free(hs);
    }
    return ERR_OK;
}

static err_t http_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{

    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *req = (char *)p->payload;
    struct http_state *hs = malloc(sizeof(struct http_state));
    if (!hs) {
        pbuf_free(p);
        tcp_close(tpcb);
        return ERR_MEM;
    }

    memset(hs, 0, sizeof(struct http_state));

    hs->sent = 0;

    if (strstr(req, "GET /dados"))
    {
        uint64_t time = to_ms_since_boot(get_absolute_time())/1000;

        char json_payload[256];
        int json_len = snprintf(json_payload, sizeof(json_payload),
            "{\"temp\":%.1f,\"time\":%lu,\"humid\":%.1f,\"press\":%.1f,\"aht\":%.1f,\"bmp\":%.1f,\"alt\":%.1f}",
            avg_temperature, time, humidity, pressure*1000, aht10_temperature, bmp280_temperature, altitude);

        
        hs->len = snprintf(hs->response, sizeof(hs->response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            json_len, json_payload);
    }
    else if (strstr(req, "GET /log"))
    {
        char json_payload[256];
        int json_len = snprintf(json_payload, sizeof(json_payload),
            "{\"msg\":\"%s\"}",
            msg);
        
        hs->len = snprintf(hs->response, sizeof(hs->response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            json_len, json_payload);
    }
    else if (strstr(req, "GET /configurar-limites?")) {
        float temp_offsetAHT10, temp_offsetBMP280, temp_offsetUmid, temp_offsetPress;

        sscanf(req,
            "GET /configurar-limites?"
            "minAHT10=%f&maxAHT10=%f&offsetAHT10=%f&"
            "minBMP280=%f&maxBMP280=%f&offsetBMP280=%f&"
            "minUmid=%f&maxUmid=%f&offsetUmid=%f&"
            "minPress=%f&maxPress=%f&offsetPress=%f",
            &minAHT10, &maxAHT10, &temp_offsetAHT10,
            &minBMP280, &maxBMP280, &temp_offsetBMP280,
            &minUmid, &maxUmid, &temp_offsetUmid,
            &minPress, &maxPress, &temp_offsetPress);

        offsetAHT10   += temp_offsetAHT10;
        offsetBMP280  += temp_offsetBMP280;
        offsetUmid    += temp_offsetUmid;
        offsetPress   += temp_offsetPress;


        hs->len = snprintf(hs->response, sizeof(hs->response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 2\r\n"
            "Connection: close\r\n"
            "\r\n"
            "OK");
    }
    else if (strstr(req, "GET /zerar")) {
        offsetAHT10  = 0;
        offsetBMP280 = 0;
        offsetUmid   = 0;
        offsetPress  = 0;

        hs->len = snprintf(hs->response, sizeof(hs->response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 2\r\n"
            "Connection: close\r\n"
            "\r\n"
            "OK");
    }
    else if(strstr(req, "GET /limites")) {
        char json_payload[512]; // aumentei o tamanho para segurança

        int json_len = snprintf(json_payload, sizeof(json_payload),
            "{"
            "\"minAHT10\":%.1f,\"maxAHT10\":%.1f,\"offsetAHT10\":%.1f,"
            "\"minBMP280\":%.1f,\"maxBMP280\":%.1f,\"offsetBMP280\":%.1f,"
            "\"minUmid\":%.1f,\"maxUmid\":%.1f,\"offsetUmid\":%.1f,"
            "\"minPress\":%.1f,\"maxPress\":%.1f,\"offsetPress\":%.1f"
            "}\r\n",
            minAHT10, maxAHT10, offsetAHT10,
            minBMP280, maxBMP280, offsetBMP280,
            minUmid, maxUmid, offsetUmid,
            minPress, maxPress, offsetPress);

        hs->len = snprintf(hs->response, sizeof(hs->response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            json_len, json_payload);
    }

    else
    {
        hs->len = snprintf(hs->response, sizeof(hs->response),
                           "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n"
                           "Content-Length: %d\r\n"
                           "Connection: close\r\n"
                           "\r\n"
                           "%s",
                           (int)strlen(HTML_BODY), HTML_BODY);
    }

    tcp_arg(tpcb, hs);
    tcp_sent(tpcb, http_sent);
    tcp_write(tpcb, hs->response, hs->len, TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    pbuf_free(p);
    return ERR_OK;
}

static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    tcp_recv(newpcb, http_recv);
    return ERR_OK;
}

void start_http_server(void)
{
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB TCP\n");
        return;
    }
    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);
    printf("Servidor HTTP rodando na porta 80...\n");
}
