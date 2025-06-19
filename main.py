import pandas as pd
import tkinter as tk
from tkinter import filedialog, messagebox
import customtkinter as ctk
import os
import sys

GPL_TEXT = """\
GNU GENERAL PUBLIC LICENSE
Version 3, 29 June 2007

Copyright (C) 2025 Moritz Breier

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Full license: https://www.gnu.org/licenses/gpl-3.0.html
"""

NOTICE_TEXT = """\
Dieses Programm kann Fehler enthalten.
Die Output-Datei sollte deshalb immer geprüft werden.
"""

def resource_path(relative_path):
    """Funktion für PyInstaller-kompatible Pfadauflösung"""
    try:
        base_path = sys._MEIPASS  # wenn gepackt
    except Exception:
        base_path = os.path.abspath(".")
    return os.path.join(base_path, relative_path)

def transform_csv(input_path, output_path):
    try:
        df = pd.read_csv(input_path, sep=";", dtype=str).fillna("")

        new_rows = []
        new_header = ["Vorname", "Nachname", "Adresse"] + df.columns[6:].tolist()
        new_rows.append(new_header)

        for i in range(1, len(df)):
            row = df.iloc[i]
            shared_data = row[6:].tolist()

            if row[0].strip():
                person1 = [row[0], row[1], row[4]] + shared_data
                new_rows.append(person1)

            if row[2].strip():
                person2 = [row[2], row[3], row[5]] + shared_data
                new_rows.append(person2)

        new_df = pd.DataFrame(new_rows)
        new_df.to_csv(output_path, sep=";", index=False, header=False, encoding="utf-8-sig")
        messagebox.showinfo("Erfolg", "Datei erfolgreich verarbeitet.")
    except Exception as e:
        messagebox.showerror("Fehler", f"Ein Fehler ist aufgetreten:\n{e}")

def show_license():
    messagebox.showinfo("Lizenz – GPLv3", GPL_TEXT)

def show_notice():
    messagebox.showinfo("Achtung", NOTICE_TEXT)

def open_gui():
    ctk.set_appearance_mode("dark")
    ctk.set_default_color_theme("dark-blue")

    app = ctk.CTk()
    app.title("CSV IServ Converter")
    app.geometry("750x270")
    app.iconbitmap(resource_path("icon.ico"))

    accent_color = "#a64ca6"
    padding = {"padx": 10, "pady": 10}

    # Switch für Light-/Darkmode
    def toggle_mode():
        if mode_switch.get():
            ctk.set_appearance_mode("dark")
        else:
            ctk.set_appearance_mode("light")

    mode_switch = ctk.CTkSwitch(app, text="Darkmode", command=toggle_mode)
    mode_switch.select()  # Start im Darkmode
    mode_switch.grid(row=0, column=2, sticky="e", padx=10, pady=(10, 0))

    # Eingabedatei
    input_label = ctk.CTkLabel(app, text="Eingabedatei:")
    input_label.grid(row=1, column=0, sticky="w", **padding)

    input_entry = ctk.CTkEntry(app, width=400)
    input_entry.grid(row=1, column=1, **padding)

    def select_input_file():
        path = filedialog.askopenfilename(title="Eingabedatei wählen", filetypes=[("CSV-Dateien", "*.csv")])
        if path:
            input_entry.delete(0, tk.END)
            input_entry.insert(0, path)

    browse_input = ctk.CTkButton(app, text="Durchsuchen", command=select_input_file, corner_radius=20, fg_color=accent_color)
    browse_input.grid(row=1, column=2, **padding)

    # Ausgabedatei
    output_label = ctk.CTkLabel(app, text="Ausgabedatei:")
    output_label.grid(row=2, column=0, sticky="w", **padding)

    output_entry = ctk.CTkEntry(app, width=400)
    output_entry.grid(row=2, column=1, **padding)

    def select_output_file():
        path = filedialog.asksaveasfilename(title="Ausgabedatei wählen", defaultextension=".csv", filetypes=[("CSV-Dateien", "*.csv")])
        if path:
            output_entry.delete(0, tk.END)
            output_entry.insert(0, path)

    browse_output = ctk.CTkButton(app, text="Speichern unter", command=select_output_file, corner_radius=20, fg_color=accent_color)
    browse_output.grid(row=2, column=2, **padding)

    # Start-Button
    def run_transform():
        input_path = input_entry.get()
        output_path = output_entry.get()
        if not input_path or not output_path:
            messagebox.showwarning("Fehlende Angaben", "Bitte Eingabe- und Ausgabedatei angeben.")
            return
        transform_csv(input_path, output_path)

    run_button = ctk.CTkButton(app, text="Start", command=run_transform, width=200, corner_radius=25, fg_color=accent_color)
    run_button.grid(row=3, column=0, columnspan=3, pady=20)

    # Lizenz- und Achtung-Buttons
    license_button = ctk.CTkButton(app, text="Lizenz anzeigen", command=show_license, corner_radius=15, fg_color="#444")
    license_button.grid(row=4, column=0, sticky="w", padx=10)

    notice_button = ctk.CTkButton(app, text="Achtung", command=show_notice, corner_radius=15, fg_color="#444")
    notice_button.grid(row=4, column=1, sticky="w", padx=10)

    # Copyright
    copyright_label = ctk.CTkLabel(app, text="© 2025 Moritz Breier", font=("Arial", 10))
    copyright_label.grid(row=4, column=2, sticky="e", padx=10)

    app.mainloop()

if __name__ == "__main__":
    open_gui()
