/**
 * particles.js — Particle Systems & Visual Popups
 */

export const particles = [];

export function addBlockDebris(x, y, color) {
  for (let i = 0; i < 6; i++) {
    particles.push({
      type: "debris",
      x,
      y,
      vx: (Math.random() - 0.5) * 4,
      vy: -2 - Math.random() * 3,
      size: 3 + Math.random() * 3,
      color,
      alpha: 1,
      life: 25 + Math.random() * 10,
    });
  }
}

export function addTextPopup(x, y, text, color = "#facc15") {
  particles.push({
    type: "text",
    x,
    y,
    vx: 0,
    vy: -1.2,
    text,
    color,
    alpha: 1,
    life: 45,
  });
}

export function addSmoke(x, y) {
  for (let i = 0; i < 8; i++) {
    particles.push({
      type: "smoke",
      x: x + (Math.random() - 0.5) * 16,
      y: y + (Math.random() - 0.5) * 16,
      vx: (Math.random() - 0.5) * 1.5,
      vy: -1.5 - Math.random() * 2,
      size: 4 + Math.random() * 4,
      color: "#64748b",
      alpha: 0.8,
      life: 30,
    });
  }
}

export function updateParticles() {
  for (let i = particles.length - 1; i >= 0; i--) {
    const p = particles[i];
    p.life--;
    p.x += p.vx;
    p.y += p.vy;

    if (p.type === "debris") {
      p.vy += 0.25; // gravity
      p.alpha = Math.max(0, p.life / 35);
    } else if (p.type === "text") {
      p.alpha = Math.max(0, p.life / 45);
    } else if (p.type === "smoke") {
      p.size += 0.2;
      p.alpha = Math.max(0, p.life / 30);
    }

    if (p.life <= 0) {
      particles.splice(i, 1);
    }
  }
}

export function renderParticles(ctx) {
  for (const p of particles) {
    ctx.save();
    ctx.globalAlpha = p.alpha;

    if (p.type === "debris") {
      ctx.fillStyle = p.color;
      ctx.fillRect(p.x, p.y, p.size, p.size);
    } else if (p.type === "smoke") {
      ctx.fillStyle = p.color;
      ctx.beginPath();
      ctx.arc(p.x, p.y, p.size, 0, Math.PI * 2);
      ctx.fill();
    } else if (p.type === "text") {
      ctx.font = "8px 'Press Start 2P', monospace";
      ctx.fillStyle = "#000";
      ctx.fillText(p.text, p.x + 1, p.y + 1);
      ctx.fillStyle = p.color;
      ctx.fillText(p.text, p.x, p.y);
    }

    ctx.restore();
  }
}

