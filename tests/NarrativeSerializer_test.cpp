#include <QTest>
#include <QDebug>
#include <QSignalSpy>

#include "NarrativeSerializer.h"
#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "narrative/NarrativeSlide.h"
#include "narrative/NarrativeSlideItem.h"
#include "narrative/NarrativeSlideLabel.h"
#include "LabelStyleGroup.h"
#include "LabelStyle.h"
#include "TypesSerializer.h"
#include "Util.h"

namespace fb = VSim::FlatBuffers;
class NarrativeSerializer_test : public QObject {
	Q_OBJECT
private:

private slots:

void slideWriteRead() {
	NarrativeSlide slide;
	
	slide.setDuration(99.9f);
	slide.setTransitionDuration(42.0f);
	slide.setStayOnNode(false);

	osg::Matrix view_matrix;
	osg::Vec3 eye(10.0f, 11.0f, 12.0f);
	osg::Vec3 target(6.9f, 3.14f, -8.0f);
	osg::Vec3 up(0.0f, 1.0f, 0.0f);
	view_matrix = osg::Matrix::lookAt(eye, target, up);
	osg::Matrix camera_matrix = osg::Matrix::inverse(view_matrix);
	slide.setCameraMatrix(camera_matrix);

	// make some labels
	NarrativeSlideLabel *label1 = new NarrativeSlideLabel();
	label1->setBackground(QColor(123, 42, 99, 12));
	label1->setHtml("Label1");
	label1->setRect(QRectF(.3, .3, 10.0, 10.0));
	label1->setType(LabelType::NONE);
	label1->setVAlignInt(Qt::AlignVCenter);

	NarrativeSlideLabel *label2 = new NarrativeSlideLabel();
	label2->setHtml("Label2");

	slide.addChild(label1);
	slide.addChild(label2);

	// TODO: make some images

	// save to buffer
	flatbuffers::FlatBufferBuilder builder;
	auto o_slide = NarrativeSerializer::createNarrativeSlide(&builder, &slide);
	builder.Finish(o_slide);

	// read from buffer
	auto fb_slide = flatbuffers::GetRoot<fb::Slide>(builder.GetBufferPointer());

	NarrativeSlide nslide;
	NarrativeSerializer::readNarrativeSlide(fb_slide, &nslide);

	// check
	QCOMPARE(nslide.getNumChildren(), 2);
	QCOMPARE(slide.getDuration(), nslide.getDuration());
	QCOMPARE(slide.getStayOnNode(), nslide.getStayOnNode());
	QCOMPARE(slide.getTransitionDuration(), nslide.getTransitionDuration());
	QVERIFY(Util::osgMatrixEq(slide.getCameraMatrix(), nslide.getCameraMatrix(), .0001));

	// check labels
	NarrativeSlideLabel * nlabel1 = dynamic_cast<NarrativeSlideLabel*>(nslide.child(0));
	QCOMPARE(label1->getBackground(), nlabel1->getBackground());
	QCOMPARE(label1->getType(), nlabel1->getType());
	QCOMPARE(label1->getVAlign(), nlabel1->getVAlign());
	QCOMPARE(label1->getHtml(), nlabel1->getHtml());

	// check rectangle
	QRectF r = label1->getRect();
	QRectF nr = nlabel1->getRect();
	float epsilon = .0001f;
	QVERIFY(fabs(r.x() - nr.x()) < epsilon);
	QVERIFY(fabs(r.y() - nr.y()) < epsilon);
	QVERIFY(fabs(r.width() - nr.width()) < epsilon);
	QVERIFY(fabs(r.height() - nr.height()) < epsilon);

	NarrativeSlideLabel * nlabel2 = dynamic_cast<NarrativeSlideLabel*>(nslide.child(1));
	QCOMPARE(nlabel2->getHtml(), label2->getHtml());
}

void narrativeGroupWriteRead() {
	osg::ref_ptr<NarrativeGroup> nars = new NarrativeGroup;

	// make some narratives
	Narrative *nar1 = new Narrative;
	nar1->setTitle("nar1");
	nar1->setAuthor("auth1");
	nar1->setDescription("desc1");
	nars->addChild(nar1);

	// edit a style
	auto styles = nar1->labelStyles();
	auto lstyle = styles->getStyle(LabelType::LABEL);
	lstyle->m_align = Qt::AlignCenter;
	lstyle->m_font_family = "cool font";
	lstyle->m_bg_color = QColor(1, 2, 3, 4);
	lstyle->m_fg_color = QColor(255, 255, 0);
	lstyle->m_underline = true;
	lstyle->m_ital = false;
	lstyle->m_weight = 33;
	lstyle->m_margin = 3;

	// make some slides
	NarrativeSlide *slide1 = new NarrativeSlide;
	slide1->setTransitionDuration(4.5f);
	nar1->addChild(slide1);

	NarrativeSlide *slide2 = new NarrativeSlide;
	slide2->setTransitionDuration(99.6f);
	nar1->addChild(slide2);

	Narrative *nar2 = new Narrative;
	nar2->setTitle("nar2");
	nars->addChild(nar2);

	// write to buffer
	flatbuffers::FlatBufferBuilder builder;
	auto o_table = NarrativeSerializer::createNarrativeTable(&builder, nars);
	builder.Finish(o_table);

	// read from buffer
	auto fb_table = flatbuffers::GetRoot<fb::NarrativeTable>(builder.GetBufferPointer());
	NarrativeGroup nnars;
	NarrativeSerializer::readNarrativeTable(fb_table, &nnars);

	// verify
	QCOMPARE(nnars.getNumChildren(), 2);

	Narrative *nnar1 = dynamic_cast<Narrative*>(nnars.child(0));
	QCOMPARE(nnar1->getTitle(), nar1->getTitle());
	QCOMPARE(nnar1->getAuthor(), nar1->getAuthor());
	QCOMPARE(nnar1->getDescription(), nar1->getDescription());

	Narrative *nnar2 = dynamic_cast<Narrative*>(nnars.child(1));
	QCOMPARE(nnar2->getTitle(), "nar2");

	// style
	auto nstyles = nnar1->labelStyles();
	auto nlstyle = nstyles->getStyle(LabelType::LABEL);
	QCOMPARE(nlstyle->m_align, Qt::AlignCenter);
	QCOMPARE(nlstyle->m_font_family, "cool font");
	QCOMPARE(nlstyle->m_bg_color, QColor(1, 2, 3, 4));
	QCOMPARE(nlstyle->m_fg_color, QColor(255, 255, 0));
	QCOMPARE(nlstyle->m_underline, true);
	QCOMPARE(nlstyle->m_ital, false);
	QCOMPARE(nlstyle->m_weight, 33);
	QCOMPARE(nlstyle->m_margin, 3);

	// slides
	QCOMPARE(nnar1->getNumChildren(), 2);
	NarrativeSlide *nslide1 = dynamic_cast<NarrativeSlide*>(nnar1->child(0));
	QCOMPARE(nslide1->getTransitionDuration(), 4.5f);

	NarrativeSlide *nslide2 = dynamic_cast<NarrativeSlide*>(nnar1->child(1));
	QCOMPARE(nslide2->getTransitionDuration(), 99.6f);

}

};

QTEST_MAIN(NarrativeSerializer_test)
#include "NarrativeSerializer_test.moc"