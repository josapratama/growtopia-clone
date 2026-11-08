# Dockerfile for 2D Sandbox MMO Netcode Engine
FROM node:20-alpine

WORKDIR /app

# Copy dependency manifests
COPY package.json ./

# Install production dependencies
RUN npm install --omit=dev

# Copy server code and public client assets
COPY server/ ./server/
COPY public/ ./public/

ENV PORT=8085
EXPOSE 8085

CMD ["node", "server/server.js"]

