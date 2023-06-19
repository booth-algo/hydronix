const char webpageCode[] =
R"=====(
<!DOCTYPE html>
<html>

<style>
	:root {
  		--light-black: #293241;
  		--bold: #ee6c4d;
  		--dark: #3D5A80;
  		--bright: #98c1d9;
  		--clear: #e0fbfc;
	}
    head{
        overscroll-behavior: none;
    }
	body{
        overscroll-behavior: none;
		background: var(--light-black);
		font-family: 'Gill Sans', 'Gill Sans MT', Calibri, 'Trebuchet MS', sans-serif ;
		color: var(--bright);
		font-size: 4vh;
        margin: 0px;
        height: 100%;
        width: 100%;
        overflow: hidden;
        position: fixed;
	}
    button{
        font-size: 100%;
        background-color: var(--light-black);
        border: none;
        color: var(--bright);
        margin: 5%;
        border: solid;
        border-width: 2px;
        padding: 5%;
        border-radius: 20px;
        border-color: var(--clear);
    }
    .buttonOn{
        border-color: var(--dark);
    }
    .buttonOff{
        border-color: var(--clear);
    }
    .title{
        position: relative;
        text-align: center;
        font-size: 160%;
        max-height: 15%;
        border-bottom: solid;
        border-color: var(--clear);
        margin: 0;
        padding: 1.5%;
    }
    .main{
        position: relative;
        display: grid;
        margin-top: 0;
        grid-template-columns: 30% 40% 30%;
        height: 85%;
    }
    .ATtitle{
        font-size: 100%;
        width: 33.3%;
    }
    .currentReadingSaved{
        background-color: var(--dark);
        color: var(--bold);
        outline: 1px;
    }
    .currentReading{
        color: var(--clear);
        background-color: var(--dark);
        outline: 1px;
    }
    #alienData{
        text-align: center;
        top: 0;
        height: 100%;
        display: grid;
    }
    #controlPannel{
        display: grid;
        column-count: 1;
        height: 100%;
        font-size: 100%;
    }
    #centreMain{
        display: grid;
        column-count: 1;
        grid-template-rows: 30% 70%;
        height: 100%;
        align-content: centre;
    }#canvas{
        position: unset;
    }#coordContainer{
        position: relative;
        top: 50%;
        text-align: center; 
    }#alienTable{
        table-layout: fixed;
        width: 100%;
        height: 100%;
        align-content: center;
        outline:1px solid;
        overflow-y: scroll;
    }#currentDataControls{
        display: grid;
        padding: 3%;
        grid-template-columns: 50% 50%; 
    }
</style>

<head>
    <title>
        Hydronix
    </title>
    <meta name="viewport" content="user-scalable=no">
</head>
<body>
    <h1 class="title" id="title">
        Hydronix
    </h1>
    <div class="main">
        <div id = "controlPannel">
            <div id="currentDataControls">
                <button onpointerdown="this.setAttribute('class','buttonOn')" onpointerout="this.setAttribute('class','buttonOff')" onclick="reset_current_sensors_btn(this)">Clear Readings</button>
                <button onpointerdown="this.setAttribute('class','buttonOn')" onpointerout="this.setAttribute('class','buttonOff')" onclick="save_readings(this)">Save Readings</button>
            </div>
            <button onpointerdown="transmitter_button(this)">Radio Mode</button>
            <button onclick="reset_page_btn()", onpointerdown="reset_page_btn_dwn(this)" onpointerout="reset_page_btn_up(this)">Reset</button>
        </div>  
        <div id = "alienData">
            <table id="alienTable">
                <tr>
                    <th class="ATtitle">Age</th>
                    <th class="ATtitle">Name</th>
                    <th class="ATtitle">Gender?</th>
                </tr>
                <tr>
                    <td class = "currentReading"> --- </td>
                    <td class = "currentReading"> --- </td>
                    <td class = "currentReading"> --- </td>
                </tr>
                <tr>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                </tr><tr>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                </tr><tr>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                </tr><tr>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                </tr><tr>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                </tr><tr>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                </tr><tr>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                </tr><tr>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                </tr><tr>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                </tr><tr>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                    <td class = "savedData"> --- </td>
                </tr>
            </table>
        </div>
        <div id = "centreMain">
            <p id = "coordContainer">
                X: <span id="x_coordinate"> </span>
                Y: <span id="y_coordinate"> </span>
            </p>
            <div>
                <canvas id="canvas" name="game"></canvas>
            </div>
        </div>
    </div>
    <script>
        // Adding websocket (port 81 is for ESP8266)
        var connection = new WebSocket(`ws://${window.location.hostname}:81`, ['arduino']);
        // Sends date to Arduino (on connection)
        connection.onopen = function () {
            console.log('WebSocket connected');
            //connection.send('Connect ' + new Date());
        };
        
        connection.onerror = function (error) {
            console.log('WebSocket Error ', error);
            
            connection = new WebSocket(`ws://${window.location.hostname}:81`, ['arduino']);
        };
       
        connection.onmessage = function (e) {
            let jsonData = JSON.parse(e.data);
            update_current_sensor_state(0,jsonData["Age"]);
            update_current_sensor_state(1,jsonData["Name"]);
            update_current_sensor_state(2,jsonData["Gender"]);
            update_current_sensor();
        };

        connection.onclose = function (){
            console.log("websocket disconected");
        }
        // Function to save joystick values

        setInterval(send, 50);
        function send(){
            var data = format_control_data();
            if(connection.readyState > 1){
                console.log("opening new socket");
                connection = new WebSocket(`ws://${window.location.hostname}:81`, ['arduino']);
            }
            connection.send(JSON.stringify(data));
            
        }
    </script>
    <script>
		const LIGHT_BLACK = "#293241";
  		const BOLD =  "#ee6c4d";
  		const DARK =  "#3D5A80";
  		const BRIGHT = "#98c1d9";
  		const CLEAR = "#e0fbfc";
        var canvas, ctx;

        window.addEventListener('load', () => {

            canvas = document.getElementById('canvas');
            ctx = canvas.getContext('2d');          
            resize(); 
            reset_page();


            document.addEventListener('mousedown', startDrawing);
            document.addEventListener('mouseup', stopDrawing);
            document.addEventListener('mousemove', Draw);

            document.addEventListener('touchstart', startDrawing);
            document.addEventListener('touchend', stopDrawing);
            document.addEventListener('touchcancel', stopDrawing);
            document.addEventListener('touchmove', Draw);
            window.addEventListener('resize', resize);

            document.getElementById("x_coordinate").innerText = 0;
            document.getElementById("y_coordinate").innerText = 0;
        });

        let touch_coord = { x: 0, y: 0 };
        let paint = false;
        var width, height, radius, x_orig, y_orig;
        var joy_colors = {
            inner : BOLD,
            outer : DARK,
            outline : BRIGHT
        };

        var page_state;

        function reset_page(){
            page_state = {
            //send to esp32
            joy_pos: [0,0],
            reciever: false,
            //determine what to store
            sensor_table_data: [],
            sensor_data_found: [],
            sensor_data: []
            };

            reset_current_sensors();
            for(let i = 0; i < 10; i++){
                page_state.sensor_table_data[i] = ["---","---","---"];
            }
            //reset physical page
            let parent = document.getElementById("controlPannel");
            let button_list = parent.getElementsByTagName("button");

            for(let i = 0; i < button_list.length; i++){
                button_list[i].setAttribute("class","buttonOff");
            }

            update_sensor_table();
            update_current_sensor();
            for(let i = 0; i < 3; i++){
                update_current_sensor_color(i);
            }
            set_joy_theme(0);
            resize();
        }

        function update_current_sensor_state(index, value){

            /*
            if (page_state.sensor_data_found[index]){
                return;
            }
            */

            if (value != null){
                page_state.sensor_data[index] = value;
                page_state.sensor_data_found[index] = true;
            }
        }

        function handle_current_data_complete(){

            for(i = 0; i < page_state.sensor_data_found.length - 1; i++){
                if (!page_state.sensor_data_found[i]){
                    console.log("incomplete set");
                    return;
                }
            }

            push_current_sensor_to_table();
            reset_current_sensors();
            update_sensor_table();
        }

        function push_current_sensor_to_table(){

            for(let i = page_state.sensor_table_data.length - 1; i > 0; i--){
                console.log(i);
                page_state.sensor_table_data[i] = page_state.sensor_table_data[i - 1];
            }
            page_state.sensor_table_data[0] = page_state.sensor_data;
        }

        function update_current_sensor(){
            let table = document.getElementById("alienTable");
            let entries = table.getElementsByClassName("currentReading");
            for(let i = 0; i < entries.length; i++){
                entries[i].innerText = page_state.sensor_data[i];
                update_current_sensor_color(i);
            }
        }

        function update_sensor_table(){
            let table = document.getElementById("alienTable");
            let entries = table.getElementsByClassName("savedData");
            for(let i = 0; i < entries.length; i++){
                entries[i].innerText = page_state.sensor_table_data[Math.floor(i/3)] [i%3];
            }
        }

        function update_current_sensor_color(i, reseting = false){
            let entries = document.getElementById("alienTable").getElementsByClassName("currentReading");
            if(reseting){
                entries[i].setAttribute("style","color: var(--light-black)");
            }else if(page_state.sensor_data_found[i] == true){
                entries[i].setAttribute("style","color: var(--bold)");
            }else{

            	if(i == 2 && page_state.sensor_data_found[0] == true && page_state.sensor_data_found[1] == true){
                		entries[i].setAttribute("style","color: var(--bold)");
            		return
            	}

                entries[i].setAttribute("style","color: var(--clear)");
            }
        }

        function format_control_data(){
            let output = {
                "x" : page_state.joy_pos[0],
                "y" : page_state.joy_pos[1],
                "joystick" : page_state.reciever
            };

            return output
        }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~BUTTON BINDS~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/* removed feature
        function lock_button(button, button_index){ //TODO: complete function
            if(page_state.pause_sensor_data[button_index] == false){
                page_state.pause_sensor_data[button_index] = true;
                button.setAttribute("class","buttonOn");
            }else{
                button.setAttribute("class","buttonOff");
                page_state.pause_sensor_data[button_index] = false;
            }
            update_current_sensor_color(button_index);
        }
*/
		
		function reset_page_btn(){
            if(confirm("are you sure you want to reset, all stored sensor readings will be lost")){
           		reset_page();
            }
        }

        function reset_page_btn_dwn(btn){
            btn.setAttribute('class','buttonOn');

            for(let i = 0; i < 3; i++){
                update_current_sensor_color(i,true);
            }
            
        }

        function reset_page_btn_up(btn){
            btn.setAttribute('class','buttonOff');

            for(let i = 0; i < 3; i++){
                update_current_sensor_color(i);
            }
            
        }

        function save_readings(btn){
            btn.setAttribute("class","buttonOff");
            handle_current_data_complete();
        }

        function reset_current_sensors_btn(btn){
            btn.setAttribute("class","buttonOff");
            reset_current_sensors();
        }

        function reset_current_sensors(){
            page_state.sensor_data_found = [false,false,false];
            page_state.sensor_data = ["---","---","---"];
            update_current_sensor();
        }

        function set_joy_theme(style){
            if(style == 0){
                joy_colors.inner = BOLD;
                joy_colors.outer = DARK;
                joy_colors.outline = BRIGHT;
            }else if(style == 1){
                joy_colors.inner = LIGHT_BLACK;
                joy_colors.outer = LIGHT_BLACK;
                joy_colors.outline = DARK;
            }
        }

        function transmitter_button(button){
            if(page_state.reciever == true){
                page_state.reciever = false;
                set_joy_theme(0);
                button.setAttribute("class","buttonOff");
            }else{
                page_state.reciever = true;
                set_joy_theme(1);
                button.setAttribute("class","buttonOn");
            }
            resize();
        }

//~~~~~~~~~~~~~~~~~~~~~~JOYSTICK CODE~~~~~~~~~~~~~~~~~~~~~~~~~~~//

        function resize() {
            width = canvas.parentElement.offsetWidth;
            radius = 100;
            height = canvas.parentElement.offsetHeight;
            x_orig = width / 2;
            y_orig = height / 2;
            ctx.canvas.width = width
            ctx.canvas.height = height;
            background();
            joystick(x_orig, y_orig);
        }

        function background() {
            let r = radius * 1; //for tinkering - i like having it set to 1
            ctx.beginPath();
            ctx.roundRect(x_orig-r, y_orig-r, r*2, r*2,r*0.1);
            ctx.fillStyle = joy_colors.outer;
            ctx.fill();
            ctx.strokeStyle = joy_colors.outline;
            ctx.lineWidth = r * 0.02;
			ctx.stroke();
        }

        function joystick(width, height) {
			let r = radius * 0.3; //feel free to tweak
            ctx.beginPath();
            ctx.arc(width, height, r, 0, Math.PI * 2, true);
            ctx.fillStyle = joy_colors.inner;
            ctx.fill();
            ctx.strokeStyle = joy_colors.outline;
            ctx.lineWidth = r * 0.07;
            ctx.stroke();
        }

        function coordsToOffset(c){
          let x = c.x - x_orig;
          let y = c.y - y_orig;
          return { x: x,y: y};
        }

        function constrain(value, constraint){
          if(value > constraint){
            return constraint;
          }
          if(value < -constraint){
            return -constraint;
          }
          return value;
        }

        function getPosition(event) {
            let mouse_x = event.clientX || event.touches[0].clientX;
            let mouse_y = event.clientY || event.touches[0].clientY;
            touch_coord.x = mouse_x - canvas.offsetLeft;
            touch_coord.y = mouse_y - (canvas.getBoundingClientRect().top);
        }

        function on_joystick() {
          let x_relative = touch_coord.x - x_orig;
          let y_relative = touch_coord.y - y_orig;
          if (x_relative < -100 || 100 < x_relative || y_relative < -100 || 100 < y_relative){
            return false
          }
          return true;
        }

        function startDrawing(event) {
            getPosition(event);
            if (on_joystick()) {
                paint = true;
                Draw(event);
            }
        }


        function stopDrawing() {
            paint = false;
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            background();
            joystick(x_orig, y_orig);
            document.getElementById("x_coordinate").innerText = 0;
            document.getElementById("y_coordinate").innerText = 0;
            page_state.joy_pos = [0,0];
        }

        function Draw(event) {

            if (paint && !page_state.reciever) {

				var x, y;

                getPosition(event);
                var touch_offset = coordsToOffset(touch_coord);

                ctx.clearRect(0, 0, canvas.width, canvas.height);
                background();

                x = Math.round(constrain(touch_offset.x, radius));
                y = Math.round(constrain(touch_offset.y, radius));
                joystick(x + x_orig, y + y_orig);    

                document.getElementById("x_coordinate").innerText =  x;
                document.getElementById("y_coordinate").innerText = y;

                page_state.joy_pos = [x,y];
            }
        } 
    </script>
</body>

</html>
)=====";
