import os
from flask import Flask, request, render_template, send_from_directory

app = Flask(__name__)


@app.route('/')
def index():
    return render_template("index.html")


@app.route('/favicon.ico')
def favicon():
    return send_from_directory(os.path.join(app.root_path, 'static'),
                          'favicon.ico', mimetype='image/vnd.microsoft.icon')


@app.route('/search')
def search():
    query = request.args.get('query')
    if query is None:
        return "Wrong request"
    search_result = [('a', i) for i in range(50)]
    return render_template("search.html", query=query, 
                                         search_result=search_result)


if __name__ == "__main__":
    app.run(debug=True)