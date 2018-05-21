
#include <QTest>

#include "Canvas/CanvasScene.h"
#include "Canvas/LabelStyleGroup.h"
#include "Canvas/LabelStyle.h"
#include "Canvas/CanvasControl.h"
#include "CanvasSerializer.h"

namespace fb = VSim::FlatBuffers;
class CanvasSerializer_test : public QObject {
	Q_OBJECT;
private slots:

	void canvasReadWrite() {
		CanvasScene scene;

		// make some labels
		auto label1 = std::make_shared<CanvasLabel>();
		label1->setBackground(QColor(123, 42, 99, 12));
		label1->setHtml("Label1");
		label1->setRect(QRectF(.3, .3, 10.0, 10.0));
		label1->setStyleType(LabelType::HEADER2);
		label1->setVAlign(Qt::AlignVCenter);
		label1->setBorderWidthPixels(2);
		label1->setBorderColor(QColor(111, 222, 33, 44));

		auto label2 = std::make_shared<CanvasLabel>();
		label2->setHtml("Label2");

		auto image = std::make_shared<CanvasImage>();
		image->setPixmap(QPixmap("assets/karnak.jpg"));
		image->setBackground(QColor(1,2,3,4));
		image->setBorderWidthPixels(2);
		image->setBorderColor(QColor(5,6,7,8));

		scene.addItem(label1);
		scene.addItem(label2);
		scene.addItem(image);

		// TODO: make some images

		// save to buffer
		flatbuffers::FlatBufferBuilder builder;
		auto o_slide = CanvasSerializer::createCanvas(&builder, &scene);
		builder.Finish(o_slide);

		// read from buffer
		auto fb_canvas = flatbuffers::GetRoot<fb::Canvas>(builder.GetBufferPointer());

		CanvasScene nscene;
		CanvasSerializer::readCanvas(fb_canvas, &nscene);

		auto items = nscene.itemList();

		QCOMPARE(items.size(), scene.items().size());

		// check labels
		CanvasLabel * nlabel1 = dynamic_cast<CanvasLabel*>(items[0].get());
		QCOMPARE(nlabel1->background(), label1->background());
		QCOMPARE(nlabel1->styleType(), label1->styleType());
		QCOMPARE(nlabel1->valign(), label1->valign());
		QCOMPARE(nlabel1->html(), label1->html());
		QCOMPARE(nlabel1->borderWidthPixels(), label1->borderWidthPixels());
		QCOMPARE(nlabel1->borderColor(), label1->borderColor());
		QCOMPARE(nlabel1->background(), label1->background());

		// check rectangle
		QRectF r = label1->rect();
		QRectF nr = nlabel1->rect();
		float epsilon = .0001f;
		QVERIFY(fabs(r.x() - nr.x()) < epsilon);
		QVERIFY(fabs(r.y() - nr.y()) < epsilon);
		QVERIFY(fabs(r.width() - nr.width()) < epsilon);
		QVERIFY(fabs(r.height() - nr.height()) < epsilon);

		CanvasLabel * nlabel2 = dynamic_cast<CanvasLabel*>(items[1].get());
		QCOMPARE(nlabel2->html(), label2->html());

		CanvasImage * nimage = dynamic_cast<CanvasImage*>(items[2].get());
		QCOMPARE(nimage->pixmap().size(), image->pixmap().size()); // how do you compare pixmaps
		QCOMPARE(nimage->borderWidthPixels(), image->borderWidthPixels());
		QCOMPARE(nimage->borderColor(), image->borderColor());
		QCOMPARE(nimage->background(), image->background());
	}
};

QTEST_MAIN(CanvasSerializer_test)
#include "CanvasSerializer_test.moc"