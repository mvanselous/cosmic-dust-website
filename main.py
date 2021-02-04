#To run this script from the terminal:
# 1) enter the virtual enviorment with the command: "source bin/activate"
# 2) "export FLASK_APP=main.py"
# 3) "export FLASK_ENV=development"
# 4) "flask run"

# 3) is called so that changes to this script are automatically pushed to the site.
###########

#By default, @app.route() only accept HTTP get request.
#Use the methods argument to add other options.
#ex: app.route('/login',methods=['GET', 'POST'])
#    def login():
#        if request.method == 'POST':
#           return login_function()
#        else:
#            return other_function()

from flask import Flask, render_template,request
import subprocess

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/form')
def form():
    return render_template('form.html')

@app.route('/calculation', methods = ['POST', 'GET'])
def calculation():
    command= 'sudo ./simple'
    
    if request.method == 'GET':
        return 'wrong request'
    if request.method == 'POST':
#         args = []
#         args.append('sudo ./simple')
#         
        form_data = request.form
        for item in form_data:
            #We can easily access both the variable name and input!
            command = " ".join([command, item, form_data.get(item)])
#             print(item)
#             print(form_data.get(item))
#             args.append(form_data.get(item))
                        
        #the form data will be replaced with the results form the calculation!
        
        #running the external c file:
        #append the command's options
#         output = subprocess.run(args,capture_output=True,shell=True)
#         output = subprocess.run(["sudo ./simple a b c"],capture_output=True,shell=True)
        output = subprocess.run(command,capture_output=True,shell=True) 
 #using shell=True may not be secure!
        #This needs to be looked into before completing the project. 
        print(output.stdout.decode("utf-8"))

        return render_template('calculation_output.html',form_data = form_data)

@app.route('/api')
def about_api():
    return render_template('api_page.html')

@app.route('/documentation')
def documentation():
    return render_template('documentation_page.html')