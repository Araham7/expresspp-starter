# Changelog

## [0.2.0] - 2026-11-19

- **Telegram bot** (cpp-telegram-bot-api): `services/telegram.service.*` (polling dev me, webhook production me, secret check) aur `bots/telegram.bot.*` (/start, /posts, /stats). `TELEGRAM_TOKEN` na ho to bot off.
- `notifyAdmin()`: naye user pe admin ko Telegram message
- `/api/v1/health` me `telegram: true/false`
- Naye env: `TELEGRAM_TOKEN`, `TELEGRAM_WEBHOOK_URL`, `TELEGRAM_WEBHOOK_SECRET`, `TELEGRAM_ADMIN_CHAT_ID`, `TELEGRAM_API_URL`
- Packages ke naye versions (expresspp 0.4.2, axiospp 0.2.2, ...), `install-deps.sh` me cpp-telegram-bot-api
- 24 tests (2 naye: Telegram, nakli Telegram API ke saath)

## [0.1.0] - 2026-11-19

Pehli release: Express++ backend starter.

- Node/Express jaisa folder structure: config, models, validators, middlewares, controllers, routes, services, jobs, utils
- Auth: register / login / logout, argon2id hashing, JWT (header ya signed httpOnly cookie), roles
- Users (me, update, avatar upload, admin list) aur Posts (CRUD, owner check, pagination / filters, populate)
- Welcome mail: bullmqpp queue → worker → nodemailerpp template
- socketiopp: JWT auth, user rooms, live `post:created`
- Bina setup ke chalta hai (memory MongoDB / queue / mail); `.env` se asli services
- helmet, morgan, cors, rate limit (login par strict), cookie-parser, static, uploads
- 22 end-to-end tests (CTest), nodemonpp config, Dockerfile + docker-compose (mongo, redis, mailpit), VS Code settings, `api.http`
