# httpserv

Servidor HTTP/1.1 escrito desde cero en C11 sobre sockets POSIX en GNU/Linux, desarrollado de manera incremental sin dependencias externas.

## Características implementadas

- **Gestión de sockets TCP/IPv4:** Creación y configuración de sockets con reutilización inmediata de puerto (`SO_REUSEADDR`) y enlazado en `0.0.0.0:4221`.
- **Parser HTTP/1.1:** Tokenización de la *Request Line* para extracción de método y ruta solicitada.
- **Enrutamiento y respuestas dinámicas:**
  - `GET /`: Responde `200 OK`.
  - `GET /echo/<texto>`: Devuelve `<texto>` como cuerpo en texto plano (`Content-Type: text/plain`) calculando dinámicamente `Content-Length`.
  - Rutas no registradas: Responde `404 Not Found`.

## Estructura del proyecto

```text
httpserv/
├── include/
│   └── parser.h      # Interfaz del parser de peticiones HTTP
├── src/
│   ├── main.c        # Ciclo de vida del socket, accept loop y enrutamiento
│   └── parser.c      # Tokenización dinámica de líneas de petición
├── Makefile
└── README.md
