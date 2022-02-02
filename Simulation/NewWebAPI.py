#!/usr/bin/env python3

from flask import Flask, request, jsonify
from threading import Thread
import time

class ChalkBot(Thread):
    LOOP_INTERVAL = 1.0 # s
    ACTION_QUEUE_CAPACITY = 5

    def __init__(self):
        Thread.__init__(self, daemon=True)

        self._pose = {
            "north": 0,
            "east": 0,
            "heading": 0,
        }

        self._action_queue = []

    @property
    def pose(self):
        return self._pose

    @property
    def action_queue(self):
        return self._action_queue

    @action_queue.setter
    def action_queue(self, new):
        assert len(new) <= ChalkBot.ACTION_QUEUE_CAPACITY
        self._action_queue = new

    def action_queue_append(self, new):
        count = min(len(new), ChalkBot.ACTION_QUEUE_CAPACITY - len(self._action_queue))
        self._action_queue += new[0:count]
        return count

    def run(self):
        print("begin")
        while True:
            self.loop()
            time.sleep(ChalkBot.LOOP_INTERVAL)
            pass

    def loop(self):
        print("loop") # todo: execute actions

app = Flask(__name__)
chalk_bot = ChalkBot()

@app.before_first_request
def before_first_request():
    chalk_bot.daemon = True
    chalk_bot.start()

@app.route('/pose', methods=['GET'])
def pose_handler():
    return jsonify(chalk_bot.pose)

@app.route('/action_queue', methods=['GET', 'PATCH'])
def action_queue_handler():
    if request.method == 'PATCH':
        data = request.json
        if data['method'] == 'replace':
            chalk_bot.action_queue = data['queue']
            return jsonify({})
        elif data['method'] == 'append':
            appended = chalk_bot.action_queue_append(data['queue'])
            return jsonify({
                "appended": appended
            })
    else:
        return jsonify({
            "capacity": ChalkBot.ACTION_QUEUE_CAPACITY,
            "queue": chalk_bot.action_queue
        })

if __name__ == '__main__':
    app.run()
