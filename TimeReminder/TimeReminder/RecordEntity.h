#ifndef RECORDENTITY_H
#define RECORDENTITY_H
#include <QString>

class RecordEntity
{
public:
    RecordEntity(){
        this->id = 0;
        this->partment = "";
        this->name = "";
        this->sex = "";
        this->depth = "";
        this->lasttime = "";
        this->deadline = "";
        this->remark = "";
    }

    RecordEntity(
            int id,
            QString partment,
            QString name,
            QString sex,
            QString depth,
            QString lasttime,
            QString deadline,
            QString remark){

        this->id = id;
        this->partment = partment;
        this->name = name;
        this->sex = sex;
        this->depth = depth;
        this->lasttime = lasttime;
        this->deadline = deadline;
        this->remark = remark;
    }

public:
    QString toString(){
        return QString("id:%1 partment:%2 name:%3 sex:%4 depth:%5 lasttime:%6 deadline:%7 remark:%8")
                .arg(id)
                .arg(partment)
                .arg(name)
                .arg(sex)
                .arg(depth)
                .arg(lasttime)
                .arg(deadline)
                .arg(remark);
    }

private:
    int id;
    QString partment;
    QString name;
    QString sex;
    QString depth;
    QString lasttime;
    QString deadline;
    QString remark;

public:
    inline QString getPara(int index) const{
        Q_ASSERT(index>0&&index<9);
        switch (index) {
        case 1:return getPartment();
        case 2:return getName();
        case 3:return getSex();
        case 4:return getDepth();
        case 5:return getLasttime();
        case 6:return getDeadline();
        case 7:return getRemark();
        default:return "";
        }
    }
    inline int getId()const{return id;}
    inline QString getPartment() const {return partment;}
    inline QString getName() const {return name;}
    inline QString getSex() const {return sex;}
    inline QString getDepth() const {return depth;}
    inline QString getLasttime() const {return lasttime;}
    inline QString getDeadline() const {return deadline;}
    inline QString getRemark() const {return remark;}

    inline void setId(int id){this->id = id;}
    inline void setPartment(const QString& partment){this->partment = partment;}
    inline void setName(const QString& name){this->name = name;}
    inline void setSex(const QString& sex){this->sex = sex;}
    inline void setDepth(const QString& depth){this->depth = depth;}
    inline void setLasttime(const QString& lasttime){this->lasttime = lasttime;}
    inline void setDeadline(const QString& deadline){this->deadline = deadline;}
    inline void setRemark(const QString& remark){this->remark = remark;}

public:
    RecordEntity& operator=(const RecordEntity* rh){
        this->id = rh->id;
        this->partment = rh->partment;
        this->name = rh->name;
        this->sex = rh->sex;
        this->depth = rh->depth;
        this->lasttime = rh->lasttime;
        this->deadline = rh->deadline;
        this->remark = rh->remark;
    }

};

#endif // RECORDENTITY_H
