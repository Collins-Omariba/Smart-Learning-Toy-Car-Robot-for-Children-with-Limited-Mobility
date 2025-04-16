from flask import Flask, request, render_template_string, redirect, url_for

app = Flask(__name__)

@app.route('/')
def index():
    # HTML template with embedded CSS for a child-friendly design
    template = '''
    <style>
        body {
            font-family: 'Baloo 2', cursive;
            background-color: #f0f8ff; /* Light blue background */
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        .age-form {
            background-color: white;
            padding: 20px;
            border: 2px solid #4682b4; /* Blue border */
            border-radius: 10px;
            width: 300px;
            box-shadow: 0 4px 8px rgba(0,0,0,0.1); /* Subtle shadow */
        }
        h1 {
            background: linear-gradient(to right, #ff7e5f, #feb47b); /* Gradient text */
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
            background-color: #32cd32; /* Bright green button */
            color: white;
            padding: 10px 20px;
            font-size: 18px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            margin-top: 10px;
        }
        input[type="submit"]:hover {
            background-color: #228b22; /* Darker green on hover */
        }
        .success {
            background-color: #90ee90; /* Light green for success */
            padding: 10px;
            margin-bottom: 20px;
            border-radius: 5px;
        }
        .error {
            background-color: #ff6347; /* Tomato red for errors */
            padding: 10px;
            margin-bottom: 20px;
            border-radius: 5px;
            color: white;
        }
    </style>
    <link href="https://fonts.googleapis.com/css2?family=Baloo+2:wght@400;700&display=swap" rel="stylesheet">
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
    </div>
    '''
    # Handle success or error messages from query parameters
    success_message = "Age has been set successfully!" if request.args.get('success') else ""
    error_message = "Invalid age. Please enter a number between 1 and 18." if request.args.get('error') == 'invalid_age' else ""
    return render_template_string(template, success_message=success_message, error_message=error_message)

@app.route('/set_age', methods=['POST'])
def set_age():
    try:
        age = int(request.form['age'])
        if age < 1 or age > 18:
            return redirect(url_for('index', error='invalid_age'))
        with open('/home/fyp213/age.txt', 'w') as f:
            f.write(str(age))
        return redirect(url_for('index', success=1))
    except ValueError:
        return redirect(url_for('index', error='invalid_age'))

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)