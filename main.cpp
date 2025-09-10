#include <QApplication>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QIcon>
#include <QPalette>
#include <QSettings>
#include <QFrame>
#include <QTextCodec>

const QString GPL_TEXT = R"(GNU GENERAL PUBLIC LICENSE
Version 3, 29 June 2007
Copyright (C) 2025 Moritz Breier
This program is free software...
Full license: https://www.gnu.org/licenses/gpl-3.0.html)";

const QString NOTICE_TEXT = R"(Dieses Programm kann Fehler enthalten.
Die Output-Datei sollte deshalb immer geprüft werden.)";

void transform_csv(const QString &input_path, const QString &output_path) {
    QFile inFile(input_path);
    QFile outFile(output_path);

    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Fehler", "Konnte Eingabedatei nicht öffnen.");
        return;
    }
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Fehler", "Konnte Ausgabedatei nicht schreiben.");
        return;
    }

    QTextStream in(&inFile);
    QTextStream out(&outFile);
    
    // UTF-8 Encoding für Umlaute
    in.setCodec("UTF-8");
    out.setCodec("UTF-8");
    
    QStringList headers;
    QList<QStringList> new_rows;

    bool firstLine = true;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList row = line.split(";");

        if (firstLine) {
            headers = row.mid(6);
            new_rows.append(QStringList{"Vorname", "Nachname", "Adresse"} + headers);
            firstLine = false;
            continue;
        }

        QStringList shared = row.mid(6);
        if (!row[0].trimmed().isEmpty()) {
            new_rows.append(QStringList{row[0], row[1], row[4]} + shared);
        }
        if (!row[2].trimmed().isEmpty()) {
            new_rows.append(QStringList{row[2], row[3], row[5]} + shared);
        }
    }

    for (const auto &row : new_rows) {
        out << row.join(";") << "\n";
    }

    QMessageBox::information(nullptr, "Erfolg", "Datei erfolgreich verarbeitet.");
}

QString getDarkModeStyles() {
    return R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2d2d30, stop:1 #252528);
            color: #e0e0e0;
            font-family: "Segoe UI", Arial, sans-serif;
        }
        
        QMainWindow, QDialog {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2d2d30, stop:1 #252528);
        }
        
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #4a9eff, stop:1 #2d7bd6);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-weight: 600;
            font-size: 13px;
            min-height: 20px;
        }
        
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #5db0ff, stop:1 #3d8be6);
            transform: translateY(-1px);
        }
        
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2d7bd6, stop:1 #1e5aa0);
        }
        
        QPushButton:disabled {
            background: #404040;
            color: #888;
        }
        
        QLineEdit {
            background: #1e1e1e;
            color: #e0e0e0;
            border: 2px solid #404040;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 13px;
            selection-background-color: #4a9eff;
        }
        
        QLineEdit:focus {
            border-color: #4a9eff;
            background: #252525;
        }
        
        QLineEdit:hover {
            border-color: #5a5a5a;
        }
        
        QCheckBox {
            color: #e0e0e0;
            font-size: 13px;
            font-weight: 500;
        }
        
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 4px;
            border: 2px solid #404040;
            background: #1e1e1e;
        }
        
        QCheckBox::indicator:checked {
            background: #4a9eff;
            border-color: #4a9eff;
            image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTAiIGhlaWdodD0iOCIgdmlld0JveD0iMCAwIDEwIDgiIGZpbGw9Im5vbmUiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+CjxwYXRoIGQ9Ik04LjUgMUwzLjUgNkwxLjUgNCIgc3Ryb2tlPSJ3aGl0ZSIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiLz4KPC9zdmc+);
        }
        
        QCheckBox::indicator:hover {
            border-color: #5a5a5a;
        }
        
        QLabel {
            color: #e0e0e0;
            font-size: 13px;
            font-weight: 500;
        }
        
        QFrame#headerFrame {
            background: rgba(74, 158, 255, 0.1);
            border: 1px solid rgba(74, 158, 255, 0.3);
            border-radius: 10px;
            margin: 5px;
        }
        
        QMessageBox {
            background: #2d2d30;
            color: #e0e0e0;
        }
        
        QMessageBox QPushButton {
            min-width: 80px;
        }
    )";
}

QString getLightModeStyles() {
    return R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #ffffff, stop:1 #f8f9fa);
            color: #333333;
            font-family: "Segoe UI", Arial, sans-serif;
        }
        
        QMainWindow, QDialog {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #ffffff, stop:1 #f8f9fa);
        }
        
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #4a9eff, stop:1 #2d7bd6);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-weight: 600;
            font-size: 13px;
            min-height: 20px;
        }
        
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #5db0ff, stop:1 #3d8be6);
            transform: translateY(-1px);
        }
        
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2d7bd6, stop:1 #1e5aa0);
        }
        
        QPushButton:disabled {
            background: #cccccc;
            color: #666;
        }
        
        QLineEdit {
            background: white;
            color: #333333;
            border: 2px solid #e0e0e0;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 13px;
            selection-background-color: #4a9eff;
        }
        
        QLineEdit:focus {
            border-color: #4a9eff;
            background: #fafafa;
        }
        
        QLineEdit:hover {
            border-color: #c0c0c0;
        }
        
        QCheckBox {
            color: #333333;
            font-size: 13px;
            font-weight: 500;
        }
        
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 4px;
            border: 2px solid #c0c0c0;
            background: white;
        }
        
        QCheckBox::indicator:checked {
            background: #4a9eff;
            border-color: #4a9eff;
            image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTAiIGhlaWdodD0iOCIgdmlld0JveD0iMCAwIDEwIDgiIGZpbGw9Im5vbmUiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+CjxwYXRoIGQ9Ik04LjUgMUwzLjUgNkwxLjUgNCIgc3Ryb2tlPSJ3aGl0ZSIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiLz4KPC9zdmc+);
        }
        
        QCheckBox::indicator:hover {
            border-color: #a0a0a0;
        }
        
        QLabel {
            color: #333333;
            font-size: 13px;
            font-weight: 500;
        }
        
        QFrame#headerFrame {
            background: rgba(74, 158, 255, 0.05);
            border: 1px solid rgba(74, 158, 255, 0.2);
            border-radius: 10px;
            margin: 5px;
        }
        
        QMessageBox {
            background: white;
            color: #333333;
        }
        
        QMessageBox QPushButton {
            min-width: 80px;
        }
    )";
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // UTF-8 Support für Umlaute
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    
    // App-Einstellungen
    app.setOrganizationName("MoritzBreier");
    app.setApplicationName("CSV_IServ_Converter");
    
    QWidget window;
    window.setWindowTitle("CSV IServ Converter");
    window.setFixedSize(700, 400);
    window.setWindowIcon(QIcon(":/csv.ico"));

    // Settings für persistente Speicherung
    QSettings settings;
    bool darkMode = settings.value("darkMode", true).toBool();

    auto *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Header mit Titel und Dark Mode Toggle
    auto *headerFrame = new QFrame();
    headerFrame->setObjectName("headerFrame");
    auto *headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(20, 15, 20, 15);
    
    auto *titleLabel = new QLabel("CSV IServ Converter");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #4a9eff;");
    
    auto *darkModeToggle = new QCheckBox("Dark Mode");
    darkModeToggle->setChecked(darkMode);
    
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(darkModeToggle);
    
    mainLayout->addWidget(headerFrame);

    // Input Datei Sektion
    auto *inputSection = new QVBoxLayout();
    inputSection->setSpacing(10);
    
    auto *inputLabel = new QLabel("📁 Eingabedatei auswählen:");
    inputLabel->setStyleSheet("font-size: 14px; font-weight: 600;");
    
    auto *inputLayout = new QHBoxLayout();
    auto *inputLine = new QLineEdit();
    inputLine->setPlaceholderText("Wählen Sie eine CSV-Datei aus...");
    auto *browseInput = new QPushButton("📂 Durchsuchen");
    browseInput->setFixedWidth(140);
    
    inputLayout->addWidget(inputLine, 1);
    inputLayout->addWidget(browseInput);
    
    inputSection->addWidget(inputLabel);
    inputSection->addLayout(inputLayout);

    // Output Datei Sektion
    auto *outputSection = new QVBoxLayout();
    outputSection->setSpacing(10);
    
    auto *outputLabel = new QLabel("💾 Ausgabedatei festlegen:");
    outputLabel->setStyleSheet("font-size: 14px; font-weight: 600;");
    
    auto *outputLayout = new QHBoxLayout();
    auto *outputLine = new QLineEdit();
    outputLine->setPlaceholderText("Geben Sie den Speicherort an...");
    auto *browseOutput = new QPushButton("💾 Speichern unter");
    browseOutput->setFixedWidth(140);
    
    outputLayout->addWidget(outputLine, 1);
    outputLayout->addWidget(browseOutput);
    
    outputSection->addWidget(outputLabel);
    outputSection->addLayout(outputLayout);

    // Start Button
    auto *startButton = new QPushButton("🚀 Konvertierung starten");
    startButton->setStyleSheet("font-size: 16px; font-weight: bold; min-height: 40px;");

    mainLayout->addLayout(inputSection);
    mainLayout->addLayout(outputSection);
    mainLayout->addWidget(startButton);
    mainLayout->addStretch();

    // Footer mit Buttons und Copyright
    auto *footerLayout = new QHBoxLayout();
    auto *licenseBtn = new QPushButton("📄 Lizenz");
    auto *noticeBtn = new QPushButton("⚠️ Hinweis");
    
    licenseBtn->setFixedWidth(100);
    noticeBtn->setFixedWidth(100);
    
    auto *copyrightLabel = new QLabel("© 2025 Moritz Breier");
    copyrightLabel->setStyleSheet("color: #888; font-size: 11px;");
    
    footerLayout->addWidget(licenseBtn);
    footerLayout->addWidget(noticeBtn);
    footerLayout->addStretch();
    footerLayout->addWidget(copyrightLabel);

    mainLayout->addLayout(footerLayout);
    window.setLayout(mainLayout);

    // Event Handlers
    QObject::connect(browseInput, &QPushButton::clicked, [&]() {
        QString path = QFileDialog::getOpenFileName(&window, "Eingabedatei wählen", "", "CSV-Dateien (*.csv)");
        if (!path.isEmpty()) {
            inputLine->setText(path);
            // Auto-suggest output filename
            if (outputLine->text().isEmpty()) {
                QFileInfo info(path);
                QString suggestedOutput = info.absolutePath() + "/" + info.baseName() + "_converted.csv";
                outputLine->setText(suggestedOutput);
            }
        }
    });

    QObject::connect(browseOutput, &QPushButton::clicked, [&]() {
        QString path = QFileDialog::getSaveFileName(&window, "Ausgabedatei wählen", "", "CSV-Dateien (*.csv)");
        if (!path.isEmpty()) outputLine->setText(path);
    });

    QObject::connect(startButton, &QPushButton::clicked, [&]() {
        if (inputLine->text().isEmpty() || outputLine->text().isEmpty()) {
            QMessageBox::warning(&window, "⚠️ Fehlende Angaben", 
                "Bitte geben Sie sowohl eine Eingabe- als auch eine Ausgabedatei an.");
            return;
        }
        transform_csv(inputLine->text(), outputLine->text());
    });

    QObject::connect(licenseBtn, &QPushButton::clicked, [&]() {
        QMessageBox::information(&window, "📄 Lizenz – GPLv3", GPL_TEXT);
    });

    QObject::connect(noticeBtn, &QPushButton::clicked, [&]() {
        QMessageBox::information(&window, "⚠️ Wichtiger Hinweis", NOTICE_TEXT);
    });

    // Dark/Light Mode Toggle
    auto applyTheme = [&](bool isDark) {
        if (isDark) {
            window.setStyleSheet(getDarkModeStyles());
        } else {
            window.setStyleSheet(getLightModeStyles());
        }
        settings.setValue("darkMode", isDark);
    };

    // Initial theme
    applyTheme(darkMode);

    QObject::connect(darkModeToggle, &QCheckBox::toggled, applyTheme);

    window.show();
    return app.exec();
}
