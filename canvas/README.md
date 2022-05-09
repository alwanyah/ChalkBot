# CanvasServer 
CanvasServer is a graphical interaction tool for creating drawings for ChalkBot.

## Using CanvasServer

1. in a command line start the server

   ```sh
   python CanvasServer.py
   ```

2. Open your browser and enter the URL

   ```
   http://127.0.0.1:8800
   ```
   
3. Draw on the canvas with your mouse.

4. `Save to File` will save the drawing to the file `coords.json`

5. `SEND` will execute the trajectory on the ChalkBot. 
   By default the server is configured t interact with the simulated ChalkBot.
   For this run the simulator as described in [Simulator](../Simulation/README.md).
   