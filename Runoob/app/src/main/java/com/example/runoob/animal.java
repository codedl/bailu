package com.example.runoob;

public class animal {
    private String name;
    private String speak;

    public animal() {

    }

    public animal(String name, String speak) {
        this.name = name;
        this.speak = speak;
    }

    public String getName() {
        return this.name;
    }

    public String getSpeak() {
        return this.speak;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setSpeak(String speak) {
        this.speak = speak;
    }
}
