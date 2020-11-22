class Timer
{
  public:
    Timer(String name) : name(name) {}
    Timer(String name, uint16_t step) : name(name), step(step) {}

    void setStep(const uint16_t step)
    { 
      this->step = step;
    }

    void setLastTime(const uint32_t lastTime)
    {
      this->lastTime = lastTime;
    }

    boolean check(const uint32_t time)
    {
      return check(time, step);
    }

    boolean check(const uint32_t time, const uint16_t animationStepTime)
    {
      boolean returnVal;
      if(time - lastTime >= animationStepTime)
      {
        lastTime = time;
//        println("Timer " + name + ", " + String(time));
        returnVal = true;
      } 
      else 
      {
        returnVal = false;
      }
      return returnVal;
    }
    
  private:
    String name;
    uint32_t lastTime = 0;
    uint16_t step = 0;
};
