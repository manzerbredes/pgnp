#include "HalfMove.hpp"

namespace pgnp {

HalfMove::HalfMove() : count(-1), isBlack(false), NAG(0), MainLine(NULL) {}

HalfMove::~HalfMove() {
  delete MainLine;
  for (auto *move : variations) {
    delete move;
  }
}

std::string HalfMove::NestedDump(const HalfMove *m, int indent) const{
  std::stringstream ss;

  for (int i = 0; i < indent; i++) {
    ss << "   ";
  }
  ss << "  "
     << " Move=" << m->move << " Count=" << m->count << " Comment=\""
     << m->comment << "\""
     << " NAG=" << m->NAG << " IsBlack=" << m->isBlack
     << " Variations=" << m->variations.size() << std::endl;

  for (auto *var : m->variations) {
    ss << NestedDump(var, indent + 1);
  }

  if (m->MainLine != NULL) {
    ss << NestedDump(m->MainLine, indent);
  }
  return (ss.str());
}

std::string HalfMove::Dump() const { return (NestedDump(this, 0)); }

int HalfMove::GetLength() const {
  int length = 0;
  const HalfMove *m = this;
  while (m != NULL) {
    length++;
    m = m->MainLine;
  }
  return length;
}

void HalfMove::Copy(HalfMove *copy) {
  copy->count = count;
  copy->isBlack = isBlack;
  copy->move = move;
  copy->comment = comment;
  copy->NAG = NAG;

  // Copy MainLine
  if (MainLine != NULL) {
    copy->MainLine = new HalfMove();
    MainLine->Copy(copy->MainLine);
  }

  // Copy variation
  for (HalfMove *var : variations) {
    HalfMove *new_var = new HalfMove();
    copy->variations.push_back(new_var);
    var->Copy(new_var);
  }
}

HalfMove *HalfMove::GetHalfMoveAt(int distance) {
  HalfMove *tmp = this;
  while (distance > 0) {
    if (tmp == NULL) {
      throw HalfMoveOutOfRange();
    }
    distance--;
    tmp = tmp->MainLine;
  }
  return (tmp);
}

CMI::HalfMove *HalfMove::GetAsCMI(){
  CMI::HalfMove *m=new CMI::HalfMove();
  m->SetSAN(move);
  m->SetNumber(count);
  m->SetIsBlack(isBlack);
  m->SetComment(comment);
  m->SetNAG(NAG);
  if(MainLine!=NULL){
    m->SetMainline(MainLine->GetAsCMI());
  }
  for (HalfMove *var : variations) {
    m->AddVariation(var->GetAsCMI());
  }
  return m;
}

} // namespace pgnp