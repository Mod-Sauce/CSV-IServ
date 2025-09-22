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
#include <QComboBox>
#include <QIcon>
#include <QPalette>
#include <QSettings>
#include <QFrame>
#include <QStringConverter>
#include <QTextCodec>

const QString GPL_TEXT = R"(GNU GENERAL PUBLIC LICENSE
Version 3, 29 June 2007
Copyright (C) 2025 Moritz Breier
This program is free software...
Full license: https://www.gnu.org/licenses/gpl-3.0.html)";

const QString NOTICE_TEXT = R"(Dieses Programm kann Fehler enthalten.
Die Output-Datei sollte deshalb immer geprüft werden.)";

struct EncodingInfo {
    QString name;
    QString displayName;
    QStringConverter::Encoding encoding;
};

const QList<EncodingInfo> ENCODINGS = {
    {"auto", "Auto (Automatische Erkennung)", QStringConverter::Utf8},
    {"utf8", "UTF-8 (Standard Linux/Mac/Web)", QStringConverter::Utf8},
    {"utf16", "UTF-16 (Excel/Windows Export)", QStringConverter::Utf16},
    {"utf16le", "UTF-16 LE (Little Endian)", QStringConverter::Utf16LE},
    {"utf16be", "UTF-16 BE (Big Endian)", QStringConverter::Utf16BE},
    {"latin1", "ISO-8859-1 (Latin-1, Westeuropäisch)", QStringConverter::Latin1},
    {"windows1252", "Windows-1252 (Microsoft Latin-1)", QStringConverter::Latin1}, // Approximation
    {"ascii", "ASCII (nur Grundzeichen)", QStringConverter::Latin1}, // Subset of Latin1
    {"latin9", "ISO-8859-15 (Latin-9, mit €)", QStringConverter::Latin1}, // Approximation
};

QString detectEncoding(const QByteArray &rawData) {
    // BOM-Erkennung
    if (rawData.startsWith("\xEF\xBB\xBF")) return "utf8";
    if (rawData.startsWith("\xFF\xFE")) return "utf16le";
    if (rawData.startsWith("\xFE\xFF")) return "utf16be";
    
    // UTF-8 Test
    QString testUtf8 = QString::fromUtf8(rawData);
    if (!testUtf8.contains(QChar::ReplacementCharacter)) {
        // Weitere Prüfung: Enthält es typische UTF-8 Multi-Byte-Sequenzen?
        QByteArray utf8Bytes = testUtf8.toUtf8();
        if (utf8Bytes == rawData) return "utf8";
    }
    
    // ASCII Test (nur druckbare Zeichen + Steuerzeichen)
    bool isAscii = true;
    for (unsigned char byte : rawData) {
        if (byte > 127) {
            isAscii = false;
            break;
        }
    }
    if (isAscii) return "ascii";
    
    // Latin-1/Windows-1252 Test (sehr häufig bei CSV)
    QString testLatin1 = QString::fromLatin1(rawData);
    
    // Heuristische Prüfung auf typische Windows-1252 Zeichen
    int windows1252Score = 0;
    QList<unsigned char> windows1252Chars = {
        0x80, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C,
        0x8E, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 
        0x9C, 0x9E, 0x9F // Typische Windows-1252 Zeichen in 0x80-0x9F Bereich
    };
    
    for (unsigned char byte : rawData) {
        if (windows1252Chars.contains(byte)) {
            windows1252Score++;
        }
    }
    
    if (windows1252Score > 0) return "windows1252";
    
    // Fallback auf Latin-1
    return "latin1";
}

struct FileEncodingResult {
    QString text;
    QString actualEncoding;
    bool hadBOM;
};

FileEncodingResult readFileWithEncoding(const QString &filePath, const QString &encodingName) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return {"", "", false};
    }
    
    QByteArray rawData = file.readAll();
    file.close();
    
    QString actualEncoding = encodingName;
    if (encodingName == "auto") {
        actualEncoding = detectEncoding(rawData);
    }
    
    bool hadBOM = false;
    
    // BOM entfernen falls vorhanden und merken
    if (actualEncoding == "utf8" && rawData.startsWith("\xEF\xBB\xBF")) {
        rawData = rawData.mid(3);
        hadBOM = true;
    } else if ((actualEncoding == "utf16le" && rawData.startsWith("\xFF\xFE")) ||
               (actualEncoding == "utf16be" && rawData.startsWith("\xFE\xFF"))) {
        rawData = rawData.mid(2);
        hadBOM = true;
    }
    
    QString text;
    if (actualEncoding == "utf8") {
        text = QString::fromUtf8(rawData);
    } else if (actualEncoding == "utf16le") {
        text = QString::fromUtf16(reinterpret_cast<const char16_t*>(rawData.constData()), rawData.size() / 2);
    } else if (actualEncoding == "utf16be") {
        // UTF-16 BE manuell konvertieren
        QByteArray swapped;
        for (int i = 0; i < rawData.size() - 1; i += 2) {
            swapped.append(rawData[i + 1]);
            swapped.append(rawData[i]);
        }
        text = QString::fromUtf16(reinterpret_cast<const char16_t*>(swapped.constData()), swapped.size() / 2);
    } else {
        // Latin-1, Windows-1252, ASCII
        text = QString::fromLatin1(rawData);
    }
    
    return {text, actualEncoding, hadBOM};
}

void writeFileWithEncoding(const QString &filePath, const QString &text, const QString &encoding, bool includeBOM = false) {
    QFile outFile(filePath);
    if (!outFile.open(QIODevice::WriteOnly)) {
        return;
    }
    
    if (encoding == "utf8") {
        if (includeBOM) {
            outFile.write("\xEF\xBB\xBF"); // UTF-8 BOM
        }
        outFile.write(text.toUtf8());
    } else if (encoding == "utf16le") {
        if (includeBOM) {
            outFile.write("\xFF\xFE"); // UTF-16 LE BOM
        }
        QByteArray utf16Data = text.toUtf8(); // Convert to UTF-8 first
        QString utf16Text = QString::fromUtf8(utf16Data); // Then to QString
        QByteArray utf16Bytes;
        for (const QChar &c : utf16Text) {
            quint16 unicode = c.unicode();
            utf16Bytes.append(static_cast<char>(unicode & 0xFF)); // Low byte
            utf16Bytes.append(static_cast<char>((unicode >> 8) & 0xFF)); // High byte
        }
        outFile.write(utf16Bytes);
    } else if (encoding == "utf16be") {
        if (includeBOM) {
            outFile.write("\xFE\xFF"); // UTF-16 BE BOM
        }
        QByteArray utf16Data = text.toUtf8(); // Convert to UTF-8 first
        QString utf16Text = QString::fromUtf8(utf16Data); // Then to QString
        QByteArray utf16Bytes;
        for (const QChar &c : utf16Text) {
            quint16 unicode = c.unicode();
            utf16Bytes.append(static_cast<char>((unicode >> 8) & 0xFF)); // High byte first
            utf16Bytes.append(static_cast<char>(unicode & 0xFF)); // Low byte
        }
        outFile.write(utf16Bytes);
    } else {
        // Latin-1, Windows-1252, ASCII - use Latin-1 encoding
        outFile.write(text.toLatin1());
    }
    
    outFile.close();
}

void transform_csv(const QString &input_path, const QString &output_path, const QString &encoding) {
    FileEncodingResult result = readFileWithEncoding(input_path, encoding);
    
    if (result.text.isEmpty()) {
        QMessageBox::warning(nullptr, "Fehler", "Konnte Eingabedatei nicht öffnen oder lesen.");
        return;
    }

    QStringList lines = result.text.split('\n', Qt::SkipEmptyParts);
    QStringList headers;
    QList<QStringList> new_rows;
    bool firstLine = true;

    for (const QString &line : lines) {
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

    // Zusammenbauen des Outputs
    QString outputText;
    for (const auto &row : new_rows) {
        outputText += row.join(";") + "\n";
    }

    // In gleicher Kodierung wie Input speichern
    writeFileWithEncoding(output_path, outputText, result.actualEncoding, result.hadBOM);

    QString encodingInfo = result.actualEncoding.toUpper();
    if (result.hadBOM) {
        encodingInfo += " (mit BOM)";
    }
    
    QMessageBox::information(nullptr, "Erfolg", 
        QString("Datei erfolgreich verarbeitet.\nKodierung: %1").arg(encodingInfo));
}
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
        
        QComboBox {
            background: #1e1e1e;
            color: #e0e0e0;
            border: 2px solid #404040;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 13px;
            min-width: 200px;
        }
        
        QComboBox:hover {
            border-color: #5a5a5a;
        }
        
        QComboBox:focus {
            border-color: #4a9eff;
        }
        
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #e0e0e0;
            margin-right: 5px;
        }
        
        QComboBox QAbstractItemView {
            background: #1e1e1e;
            color: #e0e0e0;
            border: 1px solid #404040;
            selection-background-color: #4a9eff;
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
        
        QComboBox {
            background: white;
            color: #333333;
            border: 2px solid #e0e0e0;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 13px;
            min-width: 200px;
        }
        
        QComboBox:hover {
            border-color: #c0c0c0;
        }
        
        QComboBox:focus {
            border-color: #4a9eff;
        }
        
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #333333;
            margin-right: 5px;
        }
        
        QComboBox QAbstractItemView {
            background: white;
            color: #333333;
            border: 1px solid #e0e0e0;
            selection-background-color: #4a9eff;
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
    
    // App-Einstellungen
    app.setOrganizationName("MoritzBreier");
    app.setApplicationName("CSV_IServ_Converter");
    
    QWidget window;
    window.setWindowTitle("CSV IServ Converter");
    window.setFixedSize(750, 500);
    window.setWindowIcon(QIcon(":/csv.ico"));

    // Settings für persistente Speicherung
    QSettings settings;
    bool darkMode = settings.value("darkMode", true).toBool();
    QString lastEncoding = settings.value("encoding", "auto").toString();

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

    // Kodierung Sektion
    auto *encodingSection = new QVBoxLayout();
    encodingSection->setSpacing(10);
    
    auto *encodingLabel = new QLabel("🔤 Zeichenkodierung der Eingabedatei:");
    encodingLabel->setStyleSheet("font-size: 14px; font-weight: 600;");
    
    auto *encodingCombo = new QComboBox();
    for (const auto &encoding : ENCODINGS) {
        encodingCombo->addItem(encoding.displayName, encoding.name);
        if (encoding.name == lastEncoding) {
            encodingCombo->setCurrentIndex(encodingCombo->count() - 1);
        }
    }
    
    encodingSection->addWidget(encodingLabel);
    encodingSection->addWidget(encodingCombo);

    // Input Datei Sektion
    auto *inputSection = new QVBoxLayout();
    inputSection->setSpacing(10);
    
    auto *inputLabel = new QLabel("📁 Eingabedatei auswählen:");
    inputLabel->setStyleSheet("font-size: 14px; font-weight: 600;");
    
    auto *inputLayout = new QHBoxLayout();
    auto *inputLine = new QLineEdit();
    inputLine->setPlaceholderText("Wählen Sie eine CSV-Datei aus...");
    auto *browseInput = new QPushButton("Durchsuchen");
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
    auto *browseOutput = new QPushButton("Speichern unter");
    browseOutput->setFixedWidth(140);
    
    outputLayout->addWidget(outputLine, 1);
    outputLayout->addWidget(browseOutput);
    
    outputSection->addWidget(outputLabel);
    outputSection->addLayout(outputLayout);

    // Start Button
    auto *startButton = new QPushButton("🚀 Konvertierung starten");
    startButton->setStyleSheet("font-size: 16px; font-weight: bold; min-height: 40px;");

    mainLayout->addLayout(encodingSection);
    mainLayout->addLayout(inputSection);
    mainLayout->addLayout(outputSection);
    mainLayout->addWidget(startButton);
    mainLayout->addStretch();

    // Footer mit Buttons und Copyright
    auto *footerLayout = new QHBoxLayout();
    auto *licenseBtn = new QPushButton("📄 Lizenz");
    auto *noticeBtn = new QPushButton("⚠️ Hinweis");
    
    licenseBtn->setFixedWidth(110);
    noticeBtn->setFixedWidth(110);
    
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
        
        QString selectedEncoding = encodingCombo->currentData().toString();
        settings.setValue("encoding", selectedEncoding);
        
        transform_csv(inputLine->text(), outputLine->text(), selectedEncoding);
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
