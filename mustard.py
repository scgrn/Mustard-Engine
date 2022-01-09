import tkinter as tk
# from PIL import Image, ImakeTk

root = tk.Tk()
root.title("Mustard Engine Project Manager")

canvas = tk.Canvas(root, width = 800, height = 480)
canvas.grid(columnspan = 3, rowspan = 7)

logo = tk.PhotoImage(file="mustard.png")
tk.Label (root, image = logo).grid(row = 0, column = 1)
          
newProjectText = tk.StringVar()
newProjectButton = tk.Button(root, textvariable=newProjectText, font="Consolas")
newProjectText.set("New project")
newProjectButton.grid(column = 1, row = 2)

loadProjectText = tk.StringVar()
loadProjectButton = tk.Button(root, textvariable=loadProjectText, font="Consolas")
loadProjectText.set("Load project")
loadProjectButton.grid(column = 1, row = 3)

root.mainloop()
