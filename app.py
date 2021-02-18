#To run this script from the terminal:
# 1) enter the virtual enviorment with the command: "source bin/activate"
# 2) "export FLASK_APP=app.py"
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

from flask import Flask, render_template,request,send_file
import subprocess
import csv

app = Flask(__name__)

#I will be adding a new form page for each of the possible calculations.

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/polarization')
def polarization_form():
    return render_template('polarization_form.html')

@app.route('/size')
def size_form():
    return render_template('size_form.html')

@app.route('/color')
def color_form():
    return render_template('color_form.html')


@app.route('/calculation', methods = ['POST', 'GET'])
def calculation():
    command= 'sudo ./calc_c'
#    command= 'sudo ./simple'
    
    if request.method == 'GET':
        return 'bad request'
    if request.method == 'POST':        
        form_data = request.form
        for item in form_data:
            command = " ".join([command, form_data.get(item)])
        print(command)
                
        #see an older version (main.py) for how this command is constructed
        subprocess.run(command,shell=True) 
        #running this should create/update "output.csv"
        with open('outfile.csv', newline='') as output_file:
            csv_data = csv.reader(output_file, delimiter=",")
            table_header = ["Material 1", "Material 2"]
            table_data = []
            for idx, row in enumerate(csv_data):
                if idx ==3:
                    for h_idx, item in enumerate(row):
                        if h_idx >=2:
                            table_header.append(item)
                if idx >=4:
                    table_data.append(row)                    
            
        #now we need to read output.csv and input data to output
            return render_template('calculation_output.html',table_header = table_header, table_data = table_data)


@app.route('/documentation')
def documentation():
    return render_template('documentation_page.html')