#include "HelpDialog.h"
#include "Core/VSimInfo.h"

#include <QFile>
#include <QDebug>

HelpDialog::HelpDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	if (VSimInfo::assets().isEmpty()) {

	}

	// load up the html

	//
	reload();
}

void HelpDialog::reload()
{
	QString html = readFile(VSimInfo::assets() + "/helpdialog.html");
	QString hotkeys = readFile(VSimInfo::assets() + "/hotkeys.txt");

	if (html.isEmpty()) qInfo() << "Help html is missing";
	if (html.isEmpty()) qInfo() << "Help hotkeys is missing";

	//QString html = "<p>hello world</p><?hotkey_table>";
	//QString hotkeys =
	//	"1\ttest one thing\n"
	//	"2\ttestsecondthing\n";

	loadHelp(html, hotkeys);
}

void HelpDialog::loadHelpFile(QString html_path, QString hotkeys_path)
{
}

void HelpDialog::loadHelp(QString html, QString hotkeys)
{
	QString hotkey_html;
	hotkey_html += "<table class=\"hotkeys\">";

	// parse hotkeys
	auto lines = hotkeys.split("\n", QString::SplitBehavior::SkipEmptyParts);

	std::vector<std::pair<QString, QString>> hotkey_pairs;
	for (QString l : lines) {
		l = l.trimmed();
		if (l.length() == 0) continue;
		if (l[0] == '#') continue;
		auto split = l.split('\t');
		if (split.size() < 2) continue;
		hotkey_pairs.push_back({split[0], split[1]});
	}

	for (auto &pair : hotkey_pairs) {
		hotkey_html +=
			QString(
				"<tr>"
				"<td align=\"center\">%1</TD>"
				"<td align=\"left\">%2</TD>"
				"</tr>").arg(pair.first).arg(pair.second);
	}

	hotkey_html += "</table>";

	html.replace("<?hotkey_table>", hotkey_html);

	//qDebug() << "final html" << html;
	//qDebug() << "hotkey_html" << hotkey_html;
	ui.text->setHtml(html);
}

QString HelpDialog::readFile(QString s)
{
	QFile f(s);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return "";
	return f.readAll(); //default conversion is utf-8
}
