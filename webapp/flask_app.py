from flask import Flask, request, render_template_string, redirect, url_for, session
import subprocess
import os

app = Flask(__name__)
app.secret_key = 'your_secret_key'  # Replace with a secure key

@app.route('/')
def index():
    if 'logged_in' not in session:
        return redirect(url_for('login'))
    template = '''
    <!DOCTYPE html>
    <html>
    <head>
        <title>Smart Toy Car Robot - Set Age</title>
        <link href="https://fonts.googleapis.com/css2?family=Baloo+2:wght@400;700&display=swap" rel="stylesheet">
        <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.2/css/all.min.css" id="font-awesome-css">
        <style>
            body {
                font-family: 'Baloo 2', cursive, Arial, sans-serif;
                background-color: #f0f8ff;
                display: flex;
                justify-content: center;
                align-items: center;
                height: 100vh;
                margin: 0;
            }
            .age-form {
                background-color: white;
                padding: 20px;
                border: 2px solid #4682b4;
                border-radius: 10px;
                width: 300px;
                box-shadow: 0 4px 8px rgba(0,0,0,0.1);
                text-align: center;
            }
            h1 {
                background: linear-gradient(to right, #ff7e5f, #feb47b);
                -webkit-background-clip: text;
                -webkit-text-fill-color: transparent;
                font-size: 24px;
                margin-bottom: 20px;
            }
            label {
                display: block;
                margin-bottom: 10px;
                font-size: 18px;
                color: #333;
            }
            input[type="number"] {
                padding: 10px;
                font-size: 16px;
                border: 1px solid #ccc;
                border-radius: 5px;
                width: 100px;
            }
            input[type="submit"] {
                background-color: #32cd32;
                color: white;
                padding: 10px 20px;
                font-size: 18px;
                border: none;
                border-radius: 5px;
                cursor: pointer;
                margin-top: 10px;
            }
            input[type="submit"]:hover {
                background-color: #228b22;
            }
            .success {
                background-color: #90ee90;
                padding: 10px;
                margin-bottom: 20px;
                border-radius: 5px;
            }
            .error {
                background-color: #ff6347;
                padding: 10px;
                margin-bottom: 20px;
                border-radius: 5px;
                color: white;
            }
            .button-container {
                display: flex;
                flex-wrap: wrap;
                justify-content: space-between;
                margin-top: 15px;
            }
            .manual-button, .restart-button, .logs-button, .poweroff-button {
                color: white;
                padding: 8px 16px;
                font-size: 14px;
                border: none;
                border-radius: 5px;
                cursor: pointer;
                text-decoration: none;
                display: inline-block;
                flex: 1;
                margin: 5px;
                text-align: center;
            }
            .manual-button {
                background-color: #4682b4;
            }
            .manual-button:hover {
                background-color: #3a5f8a;
            }
            .restart-button {
                background-color: #ff8c00;
            }
            .restart-button:hover {
                background-color: #e07b00;
            }
            .logs-button {
                background-color: #6c757d;
            }
            .logs-button:hover {
                background-color: #5a6268;
            }
            .poweroff-button {
                background-color: #dc3545;
            }
            .poweroff-button:hover {
                background-color: #c82333;
            }
            @media (max-width: 600px) {
                .age-form {
                    width: 90%;
                }
                .button-container {
                    flex-direction: column;
                }
                .manual-button, .restart-button, .logs-button, .poweroff-button {
                    margin: 5px 0;
                }
            }
        </style>
    </head>
    <body>
        <div class="age-form">
            <h1>Set the Child's Age</h1>
            {% if success_message %}
            <div class="success">{{ success_message }}</div>
            {% endif %}
            {% if error_message %}
            <div class="error">{{ error_message }}</div>
            {% endif %}
            <form method="post" action="/set_age">
                <label for="age">Enter child's age:</label>
                <input type="number" id="age" name="age" min="1" max="18" required>
                <input type="submit" value="Set Age">
            </form>
            <div class="button-container">
                <a href="/manual" class="manual-button">View User Manual</a>
                <a href="/restart_services" class="restart-button">Restart Robot Services</a>
                <a href="/logs" class="logs-button">View Logs</a>
                <a href="/poweroff" class="poweroff-button" onclick="return confirm('Are you sure you want to power off the robot?')">Power Off</a>
            </div>
        </div>
    </body>
    </html>
    '''
    success_message = request.args.get('success_message', '')
    error_message = request.args.get('error_message', '')
    return render_template_string(template, success_message=success_message, error_message=error_message)

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        if request.form['password'] == '1234':  # Replace with a strong password
            session['logged_in'] = True
            return redirect(url_for('index'))
        return render_template_string('''
            <!DOCTYPE html>
            <html>
            <head>
                <title>Login</title>
                <link href="https://fonts.googleapis.com/css2?family=Baloo+2:wght@400;700&display=swap" rel="stylesheet">
                <style>
                    body {
                        font-family: 'Baloo 2', cursive, Arial, sans-serif;
                        background-color: #f0f8ff;
                        display: flex;
                        justify-content: center;
                        align-items: center;
                        height: 100vh;
                        margin: 0;
                    }
                    .login-form {
                        background-color: white;
                        padding: 20px;
                        border: 2px solid #4682b4;
                        border-radius: 10px;
                        width: 300px;
                        box-shadow: 0 4px 8px rgba(0,0,0,0.1);
                        text-align: center;
                    }
                    h1 {
                        background: linear-gradient(to right, #ff7e5f, #feb47b);
                        -webkit-background-clip: text;
                        -webkit-text-fill-color: transparent;
                        font-size: 24px;
                        margin-bottom: 20px;
                    }
                    input[type="password"] {
                        padding: 10px;
                        font-size: 16px;
                        border: 1px solid #ccc;
                        border-radius: 5px;
                        width: 200px;
                        margin-bottom: 10px;
                    }
                    input[type="submit"] {
                        background-color: #32cd32;
                        color: white;
                        padding: 10px 20px;
                        font-size: 18px;
                        border: none;
                        border-radius: 5px;
                        cursor: pointer;
                    }
                    input[type="submit"]:hover {
                        background-color: #228b22;
                    }
                    .error {
                        background-color: #ff6347;
                        padding: 10px;
                        margin-bottom: 20px;
                        border-radius: 5px;
                        color: white;
                    }
                </style>
            </head>
            <body>
                <div class="login-form">
                    <h1>Login</h1>
                    <div class="error">Invalid password</div>
                    <form method="post">
                        <input type="password" name="password" placeholder="Enter password" required>
                        <input type="submit" value="Login">
                    </form>
                </div>
            </body>
            </html>
        ''')
    return render_template_string('''
        <!DOCTYPE html>
        <html>
        <head>
            <title>Login</title>
            <link href="https://fonts.googleapis.com/css2?family=Baloo+2:wght@400;700&display=swap" rel="stylesheet">
            <style>
                body {
                    font-family: 'Baloo 2', cursive, Arial, sans-serif;
                    background-color: #f0f8ff;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    height: 100vh;
                    margin: 0;
                }
                .login-form {
                    background-color: white;
                    padding: 20px;
                    border: 2px solid #4682b4;
                    border-radius: 10px;
                    width: 300px;
                    box-shadow: 0 4px 8px rgba(0,0,0,0.1);
                    text-align: center;
                }
                h1 {
                    background: linear-gradient(to right, #ff7e5f, #feb47b);
                    -webkit-background-clip: text;
                    -webkit-text-fill-color: transparent;
                    font-size: 24px;
                    margin-bottom: 20px;
                }
                input[type="password"] {
                    padding: 10px;
                    font-size: 16px;
                    border: 1px solid #ccc;
                    border-radius: 5px;
                    width: 200px;
                    margin-bottom: 10px;
                }
                input[type="submit"] {
                    background-color: #32cd32;
                    color: white;
                    padding: 10px 20px;
                    font-size: 18px;
                    border: none;
                    border-radius: 5px;
                    cursor: pointer;
                }
                input[type="submit"]:hover {
                    background-color: #228b22;
                }
            </style>
        </head>
        <body>
            <div class="login-form">
                <h1>Login</h1>
                <form method="post">
                    <input type="password" name="password" placeholder="Enter password" required>
                    <input type="submit" value="Login">
                </form>
            </div>
        </body>
        </html>
    ''')

@app.route('/set_age', methods=['POST'])
def set_age():
    if 'logged_in' not in session:
        return redirect(url_for('login'))
    try:
        age = int(request.form['age'])
        if age < 1 or age > 18:
            return redirect(url_for('index', error_message='Invalid age. Please enter a number between 1 and 18.'))
        with open('/home/fyp213/age.txt', 'w') as f:
            f.write(str(age))
        return redirect(url_for('index', success_message='Age has been set successfully!'))
    except ValueError:
        return redirect(url_for('index', error_message='Invalid age. Please enter a number between 1 and 18.'))

@app.route('/restart_services')
def restart_services():
    if 'logged_in' not in session:
        return redirect(url_for('login'))
    try:
        subprocess.run(['sudo', 'systemctl', 'daemon-reload'], check=True)
        subprocess.run(['sudo', 'systemctl', 'restart', 'wyoming-satellite.service'], check=True)
        return redirect(url_for('index', success_message='Robot services restarted successfully!'))
    except subprocess.CalledProcessError:
        return redirect(url_for('index', error_message='Failed to restart robot services. Please try again or check logs.'))

@app.route('/logs')
def logs():
    if 'logged_in' not in session:
        return redirect(url_for('login'))
    log_file = '/home/fyp213/wyoming_custom.log'
    try:
        with open(log_file, 'r') as f:
            # Read last 100 lines
            lines = f.readlines()[-100:]
            log_content = ''.join(lines)
    except FileNotFoundError:
        log_content = 'Error: Log file not found at /home/fyp213/wyoming_custom.log'
    except PermissionError:
        log_content = 'Error: Permission denied when accessing log file. Please check file permissions.'
    except Exception as e:
        log_content = f'Error: Failed to read log file: {str(e)}'
    
    template = '''
    <!DOCTYPE html>
    <html>
    <head>
        <title>Smart Toy Car Robot - View Logs</title>
        <link href="https://fonts.googleapis.com/css2?family=Baloo+2:wght@400;700&display=swap" rel="stylesheet">
        <style>
            body {
                font-family: 'Baloo 2', cursive, Arial, sans-serif;
                background-color: #f0f8ff;
                margin: 0;
                padding: 20px;
            }
            .logs-container {
                background-color: white;
                padding: 20px;
                border: 2px solid #4682b4;
                border-radius: 10px;
                max-width: 800px;
                margin: 0 auto;
                box-shadow: 0 4px 8px rgba(0,0,0,0.1);
            }
            h1 {
                background: linear-gradient(to right, #ff7e5f, #feb47b);
                -webkit-background-clip: text;
                -webkit-text-fill-color: transparent;
                font-size: 24px;
                text-align: center;
                margin-bottom: 20px;
            }
            pre {
                background-color: #f8f9fa;
                border: 1px solid #ccc;
                border-radius: 5px;
                padding: 10px;
                max-height: 400px;
                overflow-y: auto;
                font-family: 'Courier New', monospace;
                font-size: 14px;
                white-space: pre-wrap;
                word-wrap: break-word;
            }
            .back-button {
                background-color: #32cd32;
                color: white;
                padding: 10px 20px;
                font-size: 16px;
                border: none;
                border-radius: 5px;
                cursor: pointer;
                text-decoration: none;
                display: inline-block;
                margin-top: 20px;
            }
            .back-button:hover {
                background-color: #228b22;
            }
            @media (max-width: 600px) {
                .logs-container {
                    padding: 15px;
                    max-width: 100%;
                }
                h1 {
                    font-size: 20px;
                }
                pre {
                    font-size: 12px;
                }
            }
        </style>
    </head>
    <body>
        <div class="logs-container">
            <h1>System Logs</h1>
            <pre>{{ log_content }}</pre>
            <a href="/" class="back-button">Back to Main Page</a>
        </div>
    </body>
    </html>
    '''
    return render_template_string(template, log_content=log_content)

@app.route('/poweroff')
def poweroff():
    if 'logged_in' not in session:
        return redirect(url_for('login'))
    try:
        subprocess.run(['sudo', 'poweroff'], check=True)
        return redirect(url_for('index', success_message='Powering off the Raspberry Pi...'))
    except subprocess.CalledProcessError:
        return redirect(url_for('index', error_message='Failed to power off. Please try again or check logs.'))

@app.route('/manual')
def manual():
    if 'logged_in' not in session:
        return redirect(url_for('login'))
    template = '''
    <!DOCTYPE html>
    <html>
    <head>
        <title>Smart Toy Car Robot - User Manual</title>
        <link href="https://fonts.googleapis.com/css2?family=Baloo+2:wght@400;700&display=swap" rel="stylesheet">
        <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.2/css/all.min.css" id="font-awesome-css">
        <style>
            body {
                font-family: 'Baloo 2', cursive, Arial, sans-serif;
                background-color: #f0f8ff;
                margin: 0;
                padding: 20px;
            }
            .manual-container {
                background-color: white;
                padding: 30px;
                border: 2px solid #4682b4;
                border-radius: 10px;
                max-width: 600px;
                margin: 0 auto;
                box-shadow: 0 4px 8px rgba(0,0,0,0.1);
            }
            h1 {
                background: linear-gradient(to right, #ff7e5f, #feb47b);
                -webkit-background-clip: text;
                -webkit-text-fill-color: transparent;
                font-size: 28px;
                text-align: center;
                margin-bottom: 20px;
            }
            p, li {
                color: #333;
                font-size: 16px;
                line-height: 1.5;
            }
            details {
                margin-bottom: 15px;
                border: 1px solid #ccc;
                border-radius: 5px;
                padding: 10px;
                background-color: #f9f9f9;
            }
            summary {
                cursor: pointer;
                font-weight: bold;
                color: #4682b4;
                font-size: 18px;
            }
            details[open] summary {
                color: #32cd32;
            }
            .fa-icon, .emoji-icon {
                margin-right: 10px;
                color: #4682b4;
                display: inline-block;
                width: 20px;
                text-align: center;
            }
            details[open] .fa-icon, details[open] .emoji-icon {
                color: #32cd32;
            }
            .emoji-icon {
                font-size: 18px;
            }
            .back-button {
                background-color: #32cd32;
                color: white;
                padding: 10px 20px;
                font-size: 16px;
                border: none;
                border-radius: 5px;
                cursor: pointer;
                text-decoration: none;
                display: inline-block;
                margin-top: 20px;
            }
            .back-button:hover {
                background-color: #228b22;
            }
            @media (max-width: 600px) {
                .manual-container {
                    padding: 20px;
                    max-width: 100%;
                }
                h1 {
                    font-size: 24px;
                }
                summary {
                    font-size: 16px;
                }
            }
        </style>
        <script>
            // Fallback to emojis if Font Awesome fails to load
            window.onload = function() {
                const fontAwesomeLink = document.getElementById('font-awesome-css');
                if (!fontAwesomeLink.sheet) {
                    const icons = [
                        {class: 'fa-power-off', emoji: '⚡'},
                        {class: 'fa-microphone', emoji: '🎤'},
                        {class: 'fa-bluetooth', emoji: '📱'},
                        {class: 'fa-book', emoji: '📚'},
                        {class: 'fa-circle-question', emoji: '❓'},
                        {class: 'fa-shield-halved', emoji: '🛡️'},
                        {class: 'fa-file-audio', emoji: '🎵'}
                    ];
                    icons.forEach(icon => {
                        const elements = document.getElementsByClassName(icon.class);
                        for (let el of elements) {
                            el.classList.remove('fa-icon', 'fa-solid', icon.class);
                            el.classList.add('emoji-icon');
                            el.textContent = icon.emoji;
                        }
                    });
                }
            };
        </script>
    </head>
    <body>
        <div class="manual-container">
            <h1>User Manual: Smart Toy Car Robot</h1>
            <p>Welcome to the Smart Toy Car Robot! This guide helps you and your child enjoy its fun and educational features.</p>
            
            <details>
                <summary><i class="fa-solid fa-power-off fa-icon"></i> Getting Started</summary>
                <ul>
                    <li><strong>Power On/Off</strong>: Press the DPST rocker switch to turn the robot on (LEDs turn blue). Press again to turn off.</li>
                    <li><strong>Wi-Fi Check</strong>: The robot alerts you if no Wi-Fi is detected (needed for Gemini responses).</li>
                    <li><strong>Charging</strong>: Use the provided charger for the Li-ion battery pack (≈2 hours for a full charge).</li>
                </ul>
            </details>
            
            <details>
                <summary><i class="fa-solid fa-microphone fa-icon"></i> Voice Interaction</summary>
                <ul>
                    <li><strong>Wake Word</strong>: Say “Hey Jarvis” to activate (LEDs turn green).</li>
                    <li><strong>Movement Commands</strong>: Use these to control the robot:
                        <ul>
                            <li>“Move forward” – Moves straight ahead.</li>
                            <li>“Move backwards” – Moves straight back.</li>
                            <li>“Move in a square” – Traces a square path.</li>
                            <li>“Move in a triangle” – Traces a triangular path.</li>
                            <li>“Move in a rectangle” – Traces a rectangular path.</li>
                            <li>“Go right” – Rotates right.</li>
                            <li>“Go left” – Rotates left.</li>
                        </ul>
                    </li>
                    <li><strong>Fun Commands</strong>: Try these for entertainment:
                        <ul>
                            <li>“Play the abc song” – Plays the ABC song (available offline).</li>
                            <li>“Tell a story” – Shares an interactive story.</li>
                            <li>“Sing a song” – Sings a fun tune.</li>
                        </ul>
                    </li>
                    <li><strong>Educational Commands</strong>: Try “What is 2 + 2?” for math or general knowledge.</li>
                    <li><strong>Tips</strong>: Speak clearly, 1–2 feet from the robot. Avoid loud background noise.</li>
                </ul>
            </details>
            
            <details>
                <summary><i class="fa-solid fa-bluetooth fa-icon"></i> Bluetooth App</summary>
                <ul>
                    <li><strong>Connect</strong>: Open the app, pair with “TEAM TWO WHEEL” on your Android device.</li>
                    <li><strong>Controls</strong>: Use buttons for movement (forward, backward, left, right, stop) or educational modes (quizzes, storytelling).</li>
                    <li><strong>Range</strong>: Works within 10 meters; ensure no obstacles.</li>
                </ul>
            </details>
            
            <details>
                <summary><i class="fa-solid fa-book fa-icon"></i> Educational Features</summary>
                <ul>
                    <li><strong>Quizzes</strong>: Math, spelling, or general knowledge questions tailored to the child’s age.</li>
                    <li><strong>Storytelling</strong>: Interactive stories generated based on child input.</li>
                    <li><strong>Age Customization</strong>: Set the child’s age on this web app for age-appropriate responses.</li>
                </ul>
            </details>
            
            <details>
                <summary><i class="fa-solid fa-file-audio fa-icon"></i> Offline Content</summary>
                <ul>
                    <li><strong>Alphabet Song</strong>: Say “Hey Jarvis, play the abc song” to play the ABC song, available without internet connectivity.</li>
                </ul>
            </details>
            
            <details>
                <summary><i class="fa-solid fa-circle-question fa-icon"></i> Troubleshooting</summary>
                <ul>
                    <li><strong>No Response</strong>: Ensure the robot is powered on and Wi-Fi is connected. Repeat the wake word clearly. Try restarting services using the “Restart Robot Services” button on the main page.</li>
                    <li><strong>Motor Stalls</strong>: Check for obstacles; restart the robot if needed.</li>
                    <li><strong>Bluetooth Issues</strong>: Re-pair the app with TEAM TWO WHEEL; ensure the phone is within range.</li>
                </ul>
            </details>
            
            <details>
                <summary><i class="fa-solid fa-shield-halved fa-icon"></i> Safety Tips</summary>
                <ul>
                    <li><strong>Supervision</strong>: Always supervise children under 5 during use.</li>
                    <li><strong>Environment</strong>: Avoid water, extreme heat, or rough surfaces.</li>
                    <li><strong>Maintenance</strong>: Do not open the robot; contact support for repairs.</li>
                </ul>
            </details>
            
            <a href="/" class="back-button">Back to Age Input</a>
        </div>
    </body>
    </html>
    '''
    return render_template_string(template)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)