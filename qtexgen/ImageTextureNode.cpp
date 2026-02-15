#include <ImageTextureNode.h>
#include <App.h>
#include <NodeArea.h>
#include <NodeValue.h>
#include <UI.h>
#include <Util.h>

#include <CQFilename.h>
#include <QImageReader>

#include <QFileInfo>

namespace CQTextureGen {

ImageTextureNode::
ImageTextureNode(App *app) :
 AppNode(app, "Image Texture")
{
  gui_ = new ImageTextureGUI(this);

  setChild(gui_);
  setWindowTitle(QString::fromStdString(name()));

  fitChild();

  valueOutput_ = addOutputT(new ValueOutput(this));
}

void
ImageTextureNode::
setFilename(const std::string &s)
{
  filename_ = s;

#if 1
  auto filename = QString::fromStdString(s);

  QFileInfo fi(filename);

  if (! fi.exists(filename)) {
    std::cerr << "Error: Invalid texture file '" << s << "'\n";
    return;
  }

  QImageReader imageReader(filename);

  if (! imageReader.read(&image_)) {
    std::cerr << "Error: Failed to read image from '" << s << "'\n";
    return;
  }

  if (image_.isNull()) {
    std::cerr << "Error: Failed to read image from '" << s << "'\n";
    return;
  }
#else
  image_ = QImage(QString::fromStdString(filename_));

  if (image_.isNull())
    std::cerr << "Invalid image '" << filename_ << "'\n";
#endif

  image1_ = image_;

  gui_->updateWidgets();

  invalidate();
}

NodeValue *
ImageTextureNode::
calcValue()
{
  std::cerr << "ImageTextureNode::calcValue\n";

  if (image_.isNull())
    return nullptr;

  if (! value_)
    value_ = new ColorArrayValue;

  int is = app_->textureSize();

  if (image1_.width() != is || image1_.height() != is)
    image1_ = image_.scaled(is, is);

  value_->resize(is*is);

  int ii = 0;

  for (int iy = 0; iy < is; ++iy) {
    for (int ix = 0; ix < is; ++ix) {
      auto c = image1_.pixelColor(ix, iy);

      value_->setColor(ii, QColorToRGBA(c));

      ++ii;
    }
  }

  return value_;
}

void
ImageTextureNode::
process()
{
}

//---

ImageTextureNode::ValueOutput::
ValueOutput(ImageTextureNode *node) :
 NodeOutput("Value"), node_(node)
{
}

NodeValue *
ImageTextureNode::ValueOutput::
getValue()
{
  return node_->calcValue();
}

CPoint2D
ImageTextureNode::ValueOutput::
pos() const
{
  auto *gui = node_->gui();

  return outputPos(gui, gui->valueLabel());
}

//---

ImageTextureGUI::
ImageTextureGUI(ImageTextureNode *node) :
 node_(node)
{
  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  nameEdit_ = ui.addLabelEdit("Name", new CQFilename);

  valueLabel_ = ui.addOutputLabel("Value");

  ui.addStretch();

  connectSlots(true);

  updateWidgets();
}

void
ImageTextureGUI::
updateWidgets()
{
  connectSlots(false);

  nameEdit_->setName(QString::fromStdString(node_->filename()));

  connectSlots(true);
}

void
ImageTextureGUI::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, nameEdit_, SIGNAL(filenameChanged(const QString &)),
                            this, SLOT(nameSlot(const QString &)));
}

void
ImageTextureGUI::
nameSlot(const QString &s)
{
  node_->setFilename(s.toStdString());
}

}
