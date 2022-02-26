#pragma once

#include "View.hpp"

namespace geode::view {
	enum {
	    kDefaultPadding =  5,
	};

    class GEODE_API_DLL Row : public View<Row> {
    protected:
    	float m_padding = kDefaultPadding;
    public:
    	bool format() override;

    	Row* padding(float padding) {
    		m_padding = padding;
    		return this;
    	}
    };

    class GEODE_API_DLL Column : public View<Column> {
    protected:
    	float m_padding = kDefaultPadding;
    public:
    	bool format() override;

    	Column* padding(float padding) {
    		m_padding = padding;
    		return this;
    	}
    };

    enum class MajorAlignment {
    	Horizontal,
    	Vertical
    };

    class GEODE_API_DLL Grid : public View<Grid> {
    protected:
    	float m_majorPadding = kDefaultPadding;
    	float m_minorPadding = kDefaultPadding;
    	int m_itemsPerMajor = 4;
    	MajorAlignment m_alignment = MajorAlignment::Horizontal;

    public:
    	bool format() override;

    	Grid* majorPadding(float padding) {
    		m_majorPadding = padding;
    		return this;
    	}
    	Grid* minorPadding(float padding) {
    		m_minorPadding = padding;
    		return this;
    	}
    	Grid* itemsPerMajor(int count) {
    		m_itemsPerMajor = count;
    		return this;
    	}
    	Grid* padding(float padding) {
    		m_majorPadding = padding;
    		m_minorPadding = padding;
    		return this;
    	}
    	Grid* alignment(MajorAlignment alignment) {
    		m_alignment = alignment;
    		return this;
    	}
    };
}
