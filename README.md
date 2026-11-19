# my-server: Express++ backend starter 🚀

C++20 backend ka ready-made folder structure. Bilkul Node.js / Express (MERN) project jaisa: `routes → controllers → models`, middlewares, services, background jobs aur realtime. Sab kuch hamare packages se bana hai:

| Node (npm)            | Is project me                         |
|-----------------------|---------------------------------------|
| express               | **Express++** (`express::`)           |
| dotenv, morgan, cors, helmet, body-parser, express-rate-limit, jsonwebtoken / express-jwt, bcrypt/argon2, multer, joi/zod, cookie-parser, crypto | Express++ ke andar |
| mongoose              | **Nevla** (`nevla::`)                 |
| axios                 | **axiospp** (`axios::`)               |
| nodemailer            | **nodemailerpp** (`nodemailer::`)     |
| socket.io             | **socketiopp** (`socketio::`)         |
| bullmq                | **bullmqpp** (`bullmq::`)             |
| node-telegram-bot-api | **cpp-telegram-bot-api** (`telegram::`) |
| nodemon               | **nodemonpp**                         |
| package.json          | `expresspp.json`                      |
| supertest / jest      | `tests/api.test.cc` (axiospp + CTest) |

Bina kuch install kiye chal jata hai: MongoDB ki jagah `memory://`, Redis ki jagah in-memory queue, aur SMTP ki jagah mail log me print hoti hai. Telegram bot tabhi chalta hai jab `TELEGRAM_TOKEN` diya ho. `.env` me asli URL daalo to asli services use hoti hain.

## Folder structure

```
my-server/
├── CMakeLists.txt            # build file (package.json ke "dependencies" ka kaam: find_package)
├── expresspp.json            # package.json jaisa: name, version, scripts, deps, nodemonpp settings
├── .env.example              # saare env variables (copy karke .env banao)
├── api.http                  # Postman ki jagah: VS Code REST Client se requests
├── Dockerfile                # production image (multi-stage)
├── docker-compose.yml        # mongo + redis + mailpit (+ app)
├── scripts/
│   └── install-deps.sh       # "npm install": system libs + hamare packages (deps/*.zip se)
├── deps/                     # package zips yahan rakho (expresspp.zip, nevla.zip, ...)
│
├── src/
│   ├── main.cc               # server.js: config load → services start → listen → graceful shutdown
│   ├── app.hpp / app.cc      # app.js: global middleware, static files, routes, error handlers
│   │
│   ├── config/
│   │   ├── config.*          # .env se typed settings (production me weak secrets par start hi nahi hoga)
│   │   └── db.*              # MongoDB connect (Nevla) + models register
│   │
│   ├── models/               # Mongoose models jaisa
│   │   ├── user.model.*      # name, email (unique), password (hidden), role, avatar
│   │   ├── post.model.*      # title, body, tags, published, author → User
│   │   └── index.*           # registerAll(): naya model yahan add karo
│   │
│   ├── validators/           # joi / zod schemas (express::v)
│   │   ├── auth.validator.hpp
│   │   └── post.validator.hpp
│   │
│   ├── middlewares/
│   │   ├── auth.middleware.* # requireAuth(), optionalAuth(), requireRole("admin")
│   │   └── error.middleware.*# 404 JSON + server error handler (production me details chhupata hai)
│   │
│   ├── controllers/          # asli logic (req, res)
│   │   ├── auth.controller.* # register, login, logout
│   │   ├── user.controller.* # me, update me, avatar upload, list (admin)
│   │   └── post.controller.* # CRUD + owner check + socket broadcast
│   │
│   ├── routes/               # URL → middleware → controller
│   │   ├── routes.hpp
│   │   ├── index.cc          # /api/v1 (+ /health)
│   │   ├── auth.routes.cc    # /api/v1/auth
│   │   ├── user.routes.cc    # /api/v1/users
│   │   └── post.routes.cc    # /api/v1/posts
│   │
│   ├── services/             # bahar ki duniya se baat
│   │   ├── mail.service.*    # nodemailerpp (SMTP ya log)
│   │   ├── queue.service.*   # bullmqpp queues (Redis ya memory)
│   │   ├── socket.service.*  # socketiopp: JWT auth, rooms, broadcast / toUser
│   │   ├── http.service.*    # axiospp clients (example: GitHub API)
│   │   └── telegram.service.*# Telegram bot: polling (dev) / webhook (prod), notifyAdmin()
│   │
│   ├── bots/
│   │   └── telegram.bot.*    # bot commands: /start, /posts, /stats (routes jaisa, bot ke liye)
│   │
│   ├── jobs/
│   │   └── email.job.*       # "emails" queue ka worker (template → mail)
│   │
│   └── utils/
│       ├── api_response.hpp  # {"success": true, "data": ...} ek hi shape
│       ├── token.*           # JWT sign + req se userId / role
│       └── convert.hpp       # express::Json ⇄ nevla::Value
│
├── views/emails/welcome.html # mail templates (mustache: {{name}})
├── public/index.html         # static files (/) + live feed demo (socket.io)
├── uploads/                  # multer uploads (/uploads/...)
├── logs/
└── tests/api.test.cc         # end-to-end tests: asli app start karke HTTP calls
```

**Rule simple hai:** naya feature = `models/` me model → `validators/` me schema → `controllers/` me logic → `routes/` me URL → `routes/index.cc` me mount.

## Shuru kaise karein

```bash
# 1. ek baar: packages install (package zips deps/ me rakho)
sudo ./scripts/install-deps.sh

# 2. settings
cp .env.example .env        # JWT_SECRET / COOKIE_SECRET:  openssl rand -hex 32

# 3. dev mode (save karte hi rebuild + restart)
nodemonpp

# ya haath se
cmake -S . -B build && cmake --build build -j && ./build/server
```

Browser me `http://localhost:3000` kholo (live feed page), API `http://localhost:3000/api/v1`.

Asli MongoDB / Redis / SMTP chahiye? `docker compose up -d mongo redis mailpit` chalao, phir `.env` me:
```
MONGODB_URI=mongodb://127.0.0.1:27017
REDIS_HOST=127.0.0.1
SMTP_HOST=127.0.0.1
SMTP_PORT=1025              # mailpit: saari mails http://localhost:8025 par dikhengi
```

## Scripts (`expresspp.json`)

| Kaam            | Command                                                      |
|-----------------|--------------------------------------------------------------|
| dev             | `nodemonpp`                                                  |
| build           | `cmake --build build -j`                                     |
| start           | `./build/server`                                             |
| test            | `cmake --build build -j && ctest --test-dir build --output-on-failure` |
| release build   | `cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF && cmake --build build-release -j` |

## API

| Method | URL                         | Auth          | Kya karta hai                               |
|--------|-----------------------------|---------------|---------------------------------------------|
| GET    | `/api/v1/health`            | –             | status, uptime, sockets, mails, telegram    |
| POST   | `/api/v1/auth/register`     | –             | account + JWT + signed cookie, welcome mail queue me |
| POST   | `/api/v1/auth/login`        | –             | JWT + signed httpOnly cookie                |
| POST   | `/api/v1/auth/logout`       | –             | cookie clear                                |
| GET    | `/api/v1/users/me`          | user          | apni profile                                |
| PATCH  | `/api/v1/users/me`          | user          | name badlo                                  |
| POST   | `/api/v1/users/me/avatar`   | user          | image upload (field `avatar`, max 2 MB)     |
| GET    | `/api/v1/users`             | admin         | users list (`?role=&sort=&page=&limit=`)     |
| GET    | `/api/v1/posts`             | –             | published posts (`?tags=cpp&sort=-createdAt&page=1`) |
| GET    | `/api/v1/posts/:id`         | –             | ek post (author populated)                  |
| POST   | `/api/v1/posts`             | user          | post banao (published ho to socket par `post:created`) |
| PATCH  | `/api/v1/posts/:id`         | owner / admin | update                                      |
| DELETE | `/api/v1/posts/:id`         | owner / admin | delete                                      |

Token do tarike se jata hai: `Authorization: Bearer <token>` (mobile app, Postman) **ya** signed httpOnly cookie `token` (browser, login ke baad apne aap).

Errors ek jaise: `{"error": "..."}` (+ validation me `"errors": {field: message}`). 400 bad input, 401 login chahiye, 403 permission nahi, 404 nahi mila, 409 duplicate, 429 bahut zyada requests.

Socket.io client (React / browser):
```js
const socket = io("http://localhost:3000", { transports: ["websocket"], auth: { token } });
socket.on("post:created", (post) => ...);
socket.emit("ping", null, (reply) => console.log(reply));   // "pong"
```
Server se kisi ek user ko: `api::services::socket::toUser(userId, "notification", data);`

## Telegram bot

`.env` me `TELEGRAM_TOKEN` (@BotFather se) daalo, bot server ke saath chal padega:
- **Development:** `TELEGRAM_WEBHOOK_URL` khaali → polling (koi public URL nahi chahiye)
- **Production:** `TELEGRAM_WEBHOOK_URL=https://your-domain.com/api/v1/telegram/webhook` → webhook (route khud mount hota hai, `TELEGRAM_WEBHOOK_SECRET` check hota hai)
- Commands: `/start` (tumhara chat id bhi batata hai), `/posts` (latest 5 posts), `/stats`. Naye commands `src/bots/telegram.bot.cc` me.
- `TELEGRAM_ADMIN_CHAT_ID` do (`/start` se pata chalta hai) to har naye user pe tumhe Telegram message aata hai: `services::telegram::notifyAdmin("...")` kahin se bhi.
- Bot handlers apne thread pe chalte hain, isliye unme database ke normal (blocking) calls theek hain.

## Naya resource add karna (example: comments)

1. `src/models/comment.model.hpp/.cc` (`registerComment()`), aur `models/index.cc` me call karo
2. `src/validators/comment.validator.hpp`
3. `src/controllers/comment.controller.hpp/.cc`
4. `src/routes/comment.routes.cc` me `express::Router comments()`, `routes.hpp` me declare karo, `routes/index.cc` me `r.use("/comments", comments());`
5. `tests/api.test.cc` me test

CMake `src/` ki har `.cc` file khud utha leta hai, CMakeLists.txt chhune ki zarurat nahi.

## Dhyan rakhne wali baatein

- **GCC 13:** `co_await` wali line ke andar `{ ... }` braces mat likho (compiler crash). Pehle variable banao, phir `co_await` (code me har jagah aisa hi kiya hai).
- `co_await` ke baad `const auto &x = express::validated(req)` jaisa reference mat rakho: copy lo (`express::Json body = ...`).
- User update ke liye `doc.save()` nahi, `findByIdAndUpdate` (password `select(false)` hai, save poora document validate karta hai).
- Production: `NODE_ENV=production`, aur `JWT_SECRET` / `COOKIE_SECRET` kam se kam 32 characters ke random hone chahiye, warna server start hi nahi hoga.
- `.env` kabhi commit mat karo.
- Dockerfile aur `install-deps.sh` likhe gaye hain, lekin abhi tak Docker / fresh machine par test nahi hue. Pehli baar chalao to output dekh lena.

Test kiya hua: 24 end-to-end tests (Telegram bot ek nakli Telegram API ke saath), socket.io (official Node client: token auth, ack, live `post:created`), nodemonpp rebuild + restart, aur SIGTERM par graceful shutdown.
