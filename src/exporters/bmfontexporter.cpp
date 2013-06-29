#include <QTextStream>

#include "bmfontexporter.h"
#include "../fontconfig.h"
#include "../layoutconfig.h"

BMFontExporter::BMFontExporter(QObject *parent) :
    AbstractExporter(parent)
{
    setExtension("fnt");
}

bool BMFontExporter::Export(QByteArray& out) {
    QString str;
    QTextStream stream(&str, QIODevice::WriteOnly);

    stream << "info ";
    stream << "face=\"" << fontConfig()->family() << "\" ";
    stream << "size=" << fontConfig()->size() << " ";
    stream << "bold=" << fontConfig()->bold() << " ";
    stream << "italic=" << fontConfig()->italic() << " ";
    stream << "charset=\"\"" << " ";
    stream << "unicode=1";
    stream << "stretchH=" << fontConfig()->width() << " ";
    stream << "stretchV=" << fontConfig()->height() << " ";
    stream << "smooth=" << (fontConfig()->antialiased() ? "1" : "0 ") << " ";
    stream << "aa=" << (fontConfig()->antialiased() ? "1" : "0 ") << " ";
    stream << "padding=" << layoutConfig()->offsetLeft() << ","
                         << layoutConfig()->offsetTop() << ","
                         << layoutConfig()->offsetRight() << ","
                         << layoutConfig()->offsetBottom() << " ";
    stream << "spacing=" << fontConfig()->charSpacing() << ","
                         << fontConfig()->lineSpacing() << " ";
    stream << "outline=" << (layoutConfig()->onePixelOffset() ? "1" : "0");

    stream << "\n";

    stream << "common ";
    stream << "lineHeight=" << metrics().height << " ";
    stream << "base=25" << " ";
    stream << "scaleW=" << texWidth() << " ";
    stream << "scaleH=" << texHeight() << " ";
    stream << "pages=1" << " ";
    stream << "packed=0 " << " ";
    stream << "alphaChnl=0" << " ";
    stream << "redChnl=4" << " ";
    stream << "greenChnl=4" << " ";
    stream << "blueChnl=4";

    stream << "\n";

    stream << "page ";
    stream << "id=0" << " ";
    stream << "file=" << "\"" << texFilename() << "\"" << " ";

    stream << "\n";

    stream << "chars ";
    stream << "count=" << symbols().size() << " ";

    stream << "\n";

    int kernNumber = 0;
    foreach(const Symbol& c , symbols()) {
        stream << "char ";
        stream << "id=" << c.id << " ";
        stream << "x=" << c.placeX << " ";
        stream << "y=" << c.placeY << " ";
        stream << "width=" << c.placeW << " ";
        stream << "height=" << c.placeH << " ";
        stream << "xoffset=" << c.offsetX << " ";
        stream << "yoffset=" << (metrics().height - c.offsetY) << " ";
        stream << "xadvance=" << c.advance << " ";
        stream << "page=0" << " ";
        stream << "chnl=0" << " ";
        stream << "letter=" << (c.id==32 ? "space" : QString().append(c.id)) << "\n";

        for(auto k = c.kerning.begin();k!=c.kerning.end();k++)
            kernNumber ++;

    }

    stream << "kernings ";
    stream << "count=" << kernNumber << "\n";
    foreach(const Symbol& c , symbols()) {
        for(auto k = c.kerning.begin();k!=c.kerning.end();k++) {
            stream << "kerning ";
            stream << "first=" << c.id << " ";
            stream << "second=" << k.key() << " ";
            stream << "amount=" << k.value() << "\n";
        }
    }

    out = str.toAscii();

    return true;
}

AbstractExporter* BMFontExporterFactoryFunc (QObject* parent) {
    return new BMFontExporter(parent);
}
