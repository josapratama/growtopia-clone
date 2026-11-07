/**
 * audio.js — Procedural 8-bit Web Audio Synthesizer
 */

export class SoundSynth {
  constructor() {
    this.ctx = null;
    this.enabled = true;
  }

  init() {
    if (!this.ctx) {
      const AudioCtx = window.AudioContext || window.webkitAudioContext;
      this.ctx = new AudioCtx();
    }
  }

  playTone(freq, type, duration, startVol = 0.2, endVol = 0.01) {
    if (!this.enabled) return;
    this.init();
    try {
      const osc = this.ctx.createOscillator();
      const gain = this.ctx.createGain();
      osc.type = type;
      osc.frequency.setValueAtTime(freq, this.ctx.currentTime);
      gain.gain.setValueAtTime(startVol, this.ctx.currentTime);
      gain.gain.exponentialRampToValueAtTime(endVol, this.ctx.currentTime + duration);
      osc.connect(gain);
      gain.connect(this.ctx.destination);
      osc.start();
      osc.stop(this.ctx.currentTime + duration);
    } catch (e) {}
  }

  punch() {
    this.playTone(180, "triangle", 0.08, 0.3, 0.01);
  }

  breakBlock() {
    this.playTone(120, "square", 0.12, 0.25, 0.01);
    setTimeout(() => this.playTone(90, "square", 0.15, 0.2, 0.01), 40);
  }

  placeBlock() {
    this.playTone(320, "sine", 0.09, 0.2, 0.01);
  }

  jump() {
    if (!this.enabled) return;
    this.init();
    try {
      const osc = this.ctx.createOscillator();
      const gain = this.ctx.createGain();
      osc.type = "sine";
      osc.frequency.setValueAtTime(220, this.ctx.currentTime);
      osc.frequency.exponentialRampToValueAtTime(440, this.ctx.currentTime + 0.12);
      gain.gain.setValueAtTime(0.18, this.ctx.currentTime);
      gain.gain.exponentialRampToValueAtTime(0.01, this.ctx.currentTime + 0.12);
      osc.connect(gain);
      gain.connect(this.ctx.destination);
      osc.start();
      osc.stop(this.ctx.currentTime + 0.12);
    } catch (e) {}
  }

  gem() {
    this.playTone(880, "sine", 0.1, 0.2, 0.01);
    setTimeout(() => this.playTone(1320, "sine", 0.15, 0.2, 0.01), 60);
  }

  door() {
    this.playTone(400, "triangle", 0.18, 0.25, 0.01);
    setTimeout(() => this.playTone(600, "triangle", 0.25, 0.25, 0.01), 100);
  }

  splice() {
    this.playTone(523, "sine", 0.1, 0.25, 0.01);
    setTimeout(() => this.playTone(659, "sine", 0.12, 0.25, 0.01), 70);
    setTimeout(() => this.playTone(784, "sine", 0.14, 0.25, 0.01), 140);
    setTimeout(() => this.playTone(1046, "sine", 0.2, 0.3, 0.01), 210);
  }

  lock() {
    this.playTone(300, "square", 0.15, 0.3, 0.01);
    setTimeout(() => this.playTone(200, "square", 0.2, 0.3, 0.01), 80);
  }

  trade() {
    this.playTone(440, "triangle", 0.15, 0.25, 0.01);
    setTimeout(() => this.playTone(880, "triangle", 0.25, 0.3, 0.01), 120);
  }

  drop() {
    this.playTone(250, "sine", 0.07, 0.2, 0.01);
  }

  hurt() {
    this.playTone(150, "sawtooth", 0.15, 0.35, 0.01);
    setTimeout(() => this.playTone(90, "sawtooth", 0.2, 0.35, 0.01), 60);
  }

  respawn() {
    this.playTone(350, "sine", 0.15, 0.3, 0.01);
    setTimeout(() => this.playTone(520, "sine", 0.25, 0.3, 0.01), 120);
  }

  levelup() {
    this.playTone(523, "triangle", 0.12, 0.3, 0.01);
    setTimeout(() => this.playTone(659, "triangle", 0.12, 0.3, 0.01), 80);
    setTimeout(() => this.playTone(784, "triangle", 0.14, 0.3, 0.01), 160);
    setTimeout(() => this.playTone(1046, "triangle", 0.3, 0.4, 0.01), 240);
  }
}

export const sound = new SoundSynth();

