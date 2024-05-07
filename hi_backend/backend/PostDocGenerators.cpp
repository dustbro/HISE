void BackendProcessor::registerItemGenerators()
{
	AutogeneratedDocHelpers::addItemGenerators(*this);
}

void BackendProcessor::registerContentProcessor(MarkdownContentProcessor* processor)
{
	AutogeneratedDocHelpers::registerContentProcessor(processor);
}

juce::File BackendProcessor::getCachedDocFolder() const
{
	return AutogeneratedDocHelpers::getCachedDocFolder();
}

MarkdownPreview* BackendRootWindow::createOrShowDocWindow(const MarkdownLink& link)
{
	if (docWindow == nullptr)
	{
		docWindow = new FloatingTileDocumentWindow(this);
		docWindow->setName("HISE Documentation");
		docWindow->setSize(1300, 900);

		AutogeneratedDocHelpers::createDocFloatingTile(docWindow->getRootFloatingTile(), link);

		docWindow->getRootFloatingTile()->setVital(true);
	}
	else
	{
		docWindow->toFront(true);

		auto preview = FloatingTileHelpers::findTileWithId<MarkdownPreviewPanel>(docWindow->getRootFloatingTile(), "Preview");

		preview->initPanel();
		preview->preview->renderer.gotoLink(link);
	}
		
	auto p = FloatingTileHelpers::findTileWithId<MarkdownPreviewPanel>(docWindow->getRootFloatingTile(), "Preview");
	
	p->initPanel();

	return p->preview;;

	
	
}